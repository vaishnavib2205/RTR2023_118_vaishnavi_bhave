

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
    
    // Uniform
    GLuint modelMatrixUniform ;
    GLuint viewMatrixUniform ;
    GLuint projectionMatrixUniform ;

    GLuint lightAmbientUniform ;
    GLuint lightDiffuseUniform ;
    GLuint lightSpecularUniform ;
    GLuint lightPositionUniform ;

    GLuint materialAmbientUniform ;
    GLuint materialDiffuseUniform ;
    GLuint materialSpecularUniform ;
    GLuint materialShininessUniform ;

    GLuint keyPressUniform ;

    BOOL bLightingEnable;

    GLfloat lightAmbient[4];
    GLfloat lightDiffuse[4];
    GLfloat lightSpecular[4];
    GLfloat lightPosition[4];

    GLfloat  materialAmbient[4];
    GLfloat  materialDiffuse[4];
    GLfloat  materialSpecular[4];
    GLfloat  materialShininess;

    GLfloat angleCube;

    float lAngle;

    BOOL x ;
    BOOL y ;
    BOOL z ;

    GLfloat viewPortWidth;
    GLfloat viewPortHeight;

    
    Sphere sphere;
    
    float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_textures[764];
    unsigned short sphere_elements[2280];

    GLuint gNumVertices;
    GLuint gNumElements;
    
    
    
    
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
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_NORMALS, "aNormal");


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

    // Get shader uniform locations
    modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "uModelMatrix");
    viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "uViewMatrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
    lightAmbientUniform = glGetUniformLocation(shaderProgramObject, "uLightAmbient");
    lightDiffuseUniform = glGetUniformLocation(shaderProgramObject, "uLightDiffuse");
    lightSpecularUniform = glGetUniformLocation(shaderProgramObject, "uLightSpecular");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject, "uLightPosition");
    materialAmbientUniform = glGetUniformLocation(shaderProgramObject, "uMaterialAmbient");
    materialDiffuseUniform = glGetUniformLocation(shaderProgramObject, "uMaterialDiffuse");
    materialSpecularUniform = glGetUniformLocation(shaderProgramObject, "uMaterialSpecular");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "uMaterialShininess");
    keyPressUniform = glGetUniformLocation(shaderProgramObject, "uKeyPress");


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

    materialDiffuse[0] = 1.0f;
    materialDiffuse[1] = 1.0f;
    materialDiffuse[2] = 1.0f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 1.0f;
    materialSpecular[1] = 1.0f;
    materialSpecular[2] = 1.0f;
    materialSpecular[3] = 1.0f;
    
    materialShininess =  50.0f;



    // enabling depth

    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // clear color

    // set the clear colour of window to blue

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    
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

-(void)sphere:(GLfloat)materialAmbient1 :(GLfloat)materialAmbient2 :(GLfloat)materialAmbient3 :(GLfloat)materialAmbient4  :(GLfloat)materialDiffuse1 :(GLfloat)materialDiffuse2 :(GLfloat)materialDiffuse3 :(GLfloat)materialDiffuse4  :(GLfloat)materialSpecular1 :(GLfloat)materialSpecular2 :(GLfloat)materialSpecular3 :(GLfloat)materialSpecular4  :(GLfloat)shininessValue :(GLfloat)x1 :(GLfloat)y1
{
    //    resize(400, 200);

        if (bLightingEnable == TRUE)
        {
            glUniform1i(keyPressUniform, 1);

            glUniform3fv(lightAmbientUniform, 1, lightAmbient);
            glUniform3fv(lightDiffuseUniform, 1, lightDiffuse);
            glUniform3fv(lightSpecularUniform, 1, lightSpecular);
            glUniform4fv(lightPositionUniform, 1, lightPosition);

            materialAmbient[0] = materialAmbient1;
            materialAmbient[1] = materialAmbient2;
            materialAmbient[2] = materialAmbient3;
            materialAmbient[3] = materialAmbient4;

            materialDiffuse[0] = materialDiffuse1;
            materialDiffuse[1] = materialDiffuse2;
            materialDiffuse[2] = materialDiffuse3;
            materialDiffuse[3] = materialDiffuse4;

            // specular material
            materialSpecular[0] = materialSpecular1;
            materialSpecular[1] = materialSpecular2;
            materialSpecular[2] = materialSpecular3;
            materialSpecular[3] = materialSpecular4;

            // shininess
            materialShininess = shininessValue;

            glUniform3fv(materialAmbientUniform, 1, materialAmbient);
            glUniform3fv(materialDiffuseUniform, 1, materialDiffuse);
            glUniform3fv(materialSpecularUniform, 1, materialSpecular);
            glUniform1f(materialShininessUniform, materialShininess);
        }
        else {
            glUniform1i(keyPressUniform, 0);
        }

        
        mat4 translationMatrix = mat4::identity();
        mat4 lookMatrix = mat4::identity();
        translationMatrix = vmath::translate(x1, y1, 0.0f);
        lookMatrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 4.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

        // Order of multiplication very important
        mat4 modelMatrix = translationMatrix;
    
        mat4 viewMatrix = lookMatrix;

        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

        // *** bind vao ***
        glBindVertexArray(gVao_sphere);

        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
        glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

        // *** unbind vao ***
        glBindVertexArray(0);

}


-(void)display
{

    // Code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramObject);
    
    // *************************************************************************************************************************
    // ********************     Sphere     *************************************************************************************
    // *************************************************************************************************************************
    // 1st column
    glViewport(0, viewPortHeight * (5.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.0215f :0.1745f :0.0215f :1.0f :0.07568 :0.61424 :0.07568 :1.0f :0.633 :0.727811 :0.633 :1.0f :0.6 * 128 :0.0 :0.0f];
    
//    sphere(0.0215f, 0.1745f, 0.0215f, 1.0f, 0.07568, 0.61424, 0.07568, 1.0f, 0.633, 0.727811, 0.633, 1.0f, 0.6 * 128, 0.0, 0.0f);

    glViewport(0, viewPortHeight * (4.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.135f :0.2225f :0.1575f :1.0f :0.54 :0.89 :0.63 :1.0f :0.316228 :0.316228 :0.316228 :1.0f :0.1 * 128 :0.0f :0.0f];
//    sphere(0.135f, 0.2225f, 0.1575f, 1.0f, 0.54, 0.89, 0.63, 1.0f, 0.316228, 0.316228, 0.316228, 1.0f, 0.1 * 128, 0.0f, 0.0f);
    
    glViewport(0, viewPortHeight * (3.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.05375 :0.05 :0.06625 :1.0f :0.18275 :0.17 :0.22525 :1.0f :0.332741 :0.328634 :0.346435 :1.0f :0.3 * 128 :0.0f :0.0f];
//    sphere(0.05375, 0.05, 0.06625, 1.0f, 0.18275, 0.17, 0.22525, 1.0f, 0.332741, 0.328634, 0.346435, 1.0f, 0.3 * 128, 0.0f, 0.0f);
    
    glViewport(0, viewPortHeight * (2.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.25 :0.20725 :0.20725 :1.0f :1.0 :0.829 :0.829 :1.0f :0.296648 :0.296648 :0.296648 :1.0f :0.088 * 128 :0.0f :0.0f];
//    sphere(0.25, 0.20725, 0.20725, 1.0f, 1.0, 0.829, 0.829, 1.0f, 0.296648, 0.296648, 0.296648, 1.0f, 0.088 * 128, 0.0f, 0.0f);
    
    glViewport(0, viewPortHeight * (1.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.1745, 0.01175, 0.01175, 1.0f, 0.61424, 0.04136, 0.04136, 1.0f, 0.727811, 0.626959, 0.626959, 1.0f, 0.6 * 128, 0.0f, 0.0f);
    
    
    glViewport(0, viewPortHeight * (0.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.1, 0.18725, 0.1745, 1.0f, 0.396, 0.74151, 0.69102, 1.0f, 0.297254, 0.30829, 0.306678, 1.0f, 0.1 * 128, 0.0f, 0.0f);

    // 2nd column
    glViewport(viewPortWidth * (1.0f / 4.0f), viewPortHeight * (5.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.329412, 0.223529, 0.027451, 1.0f, 0.780392, 0.568627, 0.113725, 1.0f, 0.992157, 0.941176, 0.807843, 1.0f, 0.21794872 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth * (1.0f / 4.0f), viewPortHeight * (4.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.2125, 0.1275, 0.054, 1.0f, 0.714, 0.4284, 0.18144, 1.0f, 0.393548, 0.271906, 0.166721, 1.0f, 0.2 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth * (1.0f / 4.0f), viewPortHeight * (3.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.25, 0.25, 0.25, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f, 0.774597, 0.774597, 0.774597, 1.0f, 0.6 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth * (1.0f / 4.0f), viewPortHeight * (2.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.19125, 0.0735, 0.0225, 1.0f, 0.7038, 0.27048, 0.0828, 1.0f, 0.256777, 0.137622, 0.086014, 1.0f, 0.6 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth * (1.0f / 4.0f), viewPortHeight * (1.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.24725, 0.1995, 0.0745, 1.0f, 0.75164, 0.60648, 0.22648, 1.0f, 0.628281, 0.555802, 0.366065, 1.0f, 0.4 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth * (1.0f / 4.0f), viewPortHeight * (0.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.19225, 0.19225, 0.19225, 1.0f, 0.50754, 0.50754, 0.50754, 1.0f, 0.508273, 0.508273, 0.508273, 1.0f, 0.4 * 128, 0.0f, 0.0f);
    
    // 3rd Column
    glViewport(viewPortWidth * (2.0f / 4.0f), viewPortHeight * (5.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.0, 0.0, 0.0, 1.0f, 0.01, 0.01, 0.01, 1.0f, 0.50, 0.50, 0.50, 1.0f, 0.25 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth * (2.0f / 4.0f), viewPortHeight * (4.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.0, 0.1, 0.06, 1.0f, 0.0, 0.50980392, 0.50980392, 1.0f, 0.50196078, 0.50196078, 0.50196078, 1.0f, 0.25 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth * (2.0f / 4.0f), viewPortHeight * (3.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.0, 0.0, 0.0, 1.0f, 0.1, 0.35, 0.1, 1.0f, 0.45, 0.55, 0.45, 1.0f, 0.25 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth * (2.0f / 4.0f), viewPortHeight * (2.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.0, 0.0, 0.0, 1.0f, 0.5, 0.0, 0.0, 1.0f, 0.7, 0.6, 0.6, 1.0f, 0.25 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth * (2.0f / 4.0f), viewPortHeight * (1.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.0, 0.0, 0.0, 1.0f, 0.55, 0.55, 0.55, 1.0f, 0.70, 0.70, 0.70, 1.0f, 0.25 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth * (2.0f / 4.0f), viewPortHeight * (0.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.0, 0.0, 0.0, 1.0f, 0.5, 0.5, 0.0, 1.0f, 0.60, 0.60, 0.50, 1.0f, 0.25 * 128, 0.0f, 0.0f);

    // 4th column
    glViewport(viewPortWidth * (3.0f / 4.0f), viewPortHeight * (5.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.02, 0.02, 0.02, 1.0f, 0.01, 0.01, 0.01, 1.0f, 0.4, 0.4, 0.4, 1.0f, 0.078125 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth * (3.0f / 4.0f), viewPortHeight * (4.0f / 6.0f), (GLsizei)viewPortWidth * (1.0f / 4.0f), (GLsizei)viewPortHeight * (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.0, 0.05, 0.05, 1.0f, 0.4, 0.5, 0.5, 1.0f, 0.04, 0.7, 0.7, 1.0f, 0.078125 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth* (3.0f / 4.0f), viewPortHeight* (3.0f / 6.0f), (GLsizei)viewPortWidth* (1.0f / 4.0f), (GLsizei)viewPortHeight* (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.0, 0.05, 0.0, 1.0f, 0.4, 0.5, 0.4, 1.0f, 0.04, 0.7, 0.04, 1.0f, 0.078125 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth* (3.0f / 4.0f), viewPortHeight* (2.0f / 6.0f), (GLsizei)viewPortWidth* (1.0f / 4.0f), (GLsizei)viewPortHeight* (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.05, 0.0, 0.0, 1.0f, 0.5, 0.4, 0.4, 1.0f, 0.7, 0.04, 0.04, 1.0f, 0.078125 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth* (3.0f / 4.0f), viewPortHeight* (1.0f / 6.0f), (GLsizei)viewPortWidth* (1.0f / 4.0f), (GLsizei)viewPortHeight* (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.05, 0.05, 0.05, 1.0f, 0.5, 0.5, 0.5, 1.0f, 0.7, 0.7, 0.7, 1.0f, 0.078125 * 128, 0.0f, 0.0f);
    
    glViewport(viewPortWidth* (3.0f / 4.0f), viewPortHeight* (0.0f / 6.0f), (GLsizei)viewPortWidth* (1.0f / 4.0f), (GLsizei)viewPortHeight* (1.0f / 4.0f));
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)viewPortWidth / viewPortHeight, 0.1f, 100.0f);
    [self sphere:0.1745 :0.01175 :0.01175 :1.0f :0.61424 :0.04136 :0.04136 :1.0f :0.727811 :0.626959 :0.626959 :1.0f :0.6 * 128 :0.0f :0.0f];
//    sphere(0.05, 0.05, 0.0, 1.0f, 0.5, 0.5, 0.4, 1.0f, 0.7, 0.7, 0.04, 1.0f, 0.078125 * 128, 0.0f, 0.0f);


    glUseProgram(0);



}

-(void)myupdate
{
    // Code
    angleCube = angleCube + 0.05f;
    if (angleCube > 360.0f)
    {
        angleCube = angleCube - 360.0f;
    }

    if (x)
    {
        lightPosition[0] = 0.0f;
        lightPosition[1] = 100.0f * (float)sin(lAngle);
        lightPosition[2] = 100.0f * (float)cos(lAngle);
    }
    if(y)
    {
        lightPosition[0] = 100.0f * (float)cos(lAngle);
        lightPosition[1] = 0.0f;
        lightPosition[2] = 100.0f * (float)sin(lAngle);
    }
    if (z)
    {
        lightPosition[0] = 100.0f * (float)cos(lAngle);
        lightPosition[1] = 100.0f * (float)sin(lAngle);
        lightPosition[2] = 0.0f;
    }

    lAngle += 0.005f;
    if (lAngle > (2 * M_PI))
    {
        lAngle = 0.0f;
    }

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
    bLightingEnable = TRUE;
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gestureRecognizer
{
    // Code
    x = TRUE;
    y = FALSE;
    z = FALSE;
        

}

-(void)onSwipe:(UISwipeGestureRecognizer *)gestureRecognizer
{
    // Code
    y = TRUE;
    x = FALSE;
    z = FALSE;
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gestureRecognizer
{
    // Code
    z = TRUE;
    y = FALSE;
    x = FALSE;
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
