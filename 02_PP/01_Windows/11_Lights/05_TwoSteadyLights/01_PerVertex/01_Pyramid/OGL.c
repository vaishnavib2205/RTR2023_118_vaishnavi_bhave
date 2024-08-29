#include<Windows.h> // Windows header files 
#include "OGL.h"
#include<stdlib.h> // For exit
#include<stdio.h> // For file IO (input output)

// OpenGL header files
#include<gl/glew.h> // this must be before gl/GL.h
#include<gl/GL.h>

// header file for vmath
#include "vmath.h"

using namespace vmath;

// MACROS

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Link with OpenGL libraray

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.lib")


//global function delaration



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Entry point function

// global variable declaration
FILE* gpFile = NULL;

HWND ghwnd = NULL;
BOOL gbActive = FALSE;

DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
BOOL gbFullscreen = FALSE;

//OpenGL related Global variables
HDC ghdc = NULL;   // DC
HGLRC ghrc = NULL; // rendating context

//GLuint vertexShaderObject = 0;
//GLuint fragmentShaderObject = 0;
GLuint shaderProgramObject = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMALS   // lights
};

// variables for triangle
GLuint vao_Pyramid = 0;
GLuint vbo_position_Pyramid = 0;
GLuint vbo_color_Pyramid = 0;
GLuint vbo_normal_Pyramid = 0; // lights
GLfloat angle_Pyramid = 0.0f;



GLuint ModelMatrixUniform = 0;
GLuint ViewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;

GLuint lightDiffuseUniform[2];
GLuint lightAmbientUniform[2];
GLuint lightSpecularUniform[2];
GLuint lightPositionUniform[2];

GLuint materialDiffuseUniform = 0;
GLuint materialAmbientUniform = 0;
GLuint materialSpecularUniform = 0;
GLuint materialShininessUniform = 0;

GLuint keyPressUniform = 0;

BOOL bLightingEnabled = FALSE;
BOOL bAnimationEnable = FALSE;

struct Light
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec4 position;
};

struct Light light[2];

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 128.0f;

mat4 perspectiveProjectionMatrix; // mat4 datatype is in vmath.h header file, that means 4X4 matrix

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCnmdLine, int iCmdShow) // function defition
{

	// Function Declarations
	
	int initialize(void);
	void uninitialized(void);
	void display(void);
	void update(void);

	// Local variable Declarations

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("PBWindow");
	int iResult = 0;
	BOOL bDone = FALSE;

	if (fopen_s(&gpFile , "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log file cannot be open!!!"), TEXT("File create error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "Program started successfully \n");
	fprintf(gpFile, "****************************************************************************************** \n");


	//Code
	//WNDCLASSEX initialisation

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// Register WNDCLASSEX
	RegisterClassEx(&wndclass);

	int WW = WIN_WIDTH;
	int WH = WIN_HEIGHT;

	float SW = GetSystemMetrics(SM_CXSCREEN);
	float SH = GetSystemMetrics(SM_CYSCREEN);
	int X = (SW - WW) / 2;
	int Y = (SH - WH) / 2;


	//Create Window 
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		szAppName,
		TEXT("Prasad Bhalkikar"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		X,
		Y,
		WW,
		WH,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ghwnd = hwnd;

	// initialization

	iResult = initialize();

	if (iResult != 0)
	{
		MessageBox(hwnd, TEXT("Initialize() FAILED !!!"), TEXT(" error"), MB_OK | MB_ICONERROR);
		DestroyWindow(hwnd);
		
	}

	// Show Window
	ShowWindow(hwnd, iCmdShow);

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);


	// Game Loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActive == TRUE)
			{
				// Render
				display();

				if (bAnimationEnable == TRUE)
				{
					// Update
					update();
				}	
			}
		}
	}

	//uninitialization
	uninitialized();

	return((int)msg.wParam);
}

// callBack Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration

	void resize(int, int);

	void ToggleFullscreen(void);

	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Hello World !!!");

	// code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActive = TRUE;
		break;

	case WM_KILLFOCUS:
		gbActive = FALSE;
		break;
		
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_ERASEBKGND:
		return 0;
		

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'F':
		case 'f':
			if (gbFullscreen == FALSE)
			{
				ToggleFullscreen();
				gbFullscreen = TRUE;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = FALSE;
			}
			break;

		case 'A':
		case 'a':

			if (bAnimationEnable == FALSE)
			{
				bAnimationEnable = TRUE;
			}
			else
			{
				bAnimationEnable = FALSE;
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
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));

}

void ToggleFullscreen(void)
{
	// local variable declarations

	MONITORINFO mi = { sizeof(MONITORINFO) };

	// code
	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(
					ghwnd,
					HWND_TOP, //WS_OVERLAPPED is brought back using this statement 
					mi.rcMonitor.left, // this is rect rc , that is rectasngle vala rc giving us left of monitor  
					mi.rcMonitor.top, // this is rect rc , that is rectasngle vala rc giving us top of monitor
					mi.rcMonitor.right - mi.rcMonitor.left, // right of monitor - left gives width
					mi.rcMonitor.bottom - mi.rcMonitor.top, // height of
					SWP_NOZORDER | SWP_FRAMECHANGED
				);
			}
		}
		ShowCursor(FALSE);
	}

	else
	{
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(
			ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED
		);
		ShowCursor(TRUE);
	}

}

int initialize(void)
{
	// Function Declarations

	void resize(int, int);
	void printGLInfo(void);
	void uninitialized(void);

	// Code
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR)); 	// Remaining members are kept zero in pfd struct using this.

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	// Get the DC (device context , painter)

	ghdc = GetDC(ghwnd);
	if (ghdc == NULL)
	{
		fprintf(gpFile, "GetDC() failed \n");
		return -1;
	}

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd); // iPixelFormatIndex must be always non-zero positive
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat() Failed \n");
		return -2;
	}

	// set obtained pixel format step-4

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() Failed \n");
		return -3;
	}
	// Tell Windows graphing libraray to give OpenGL compatible context from this DC
	// Create OpenGl context from device context.

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() failed \n");
		return -4;
	}
	// now ghdc will end its role and will give control to ghrc for furthrt steps
	//Make rendering context current

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() failed \n");
		return -5;
	}
	
	// initialize glew

	if (glewInit() != GLEW_OK)
	{
		fprintf(gpFile, "glewInit() failed to load GLEW \n");
		return -6;
	}

	// printGLInfo

	printGLInfo();

	// vertex shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 aPosition;" \
		"in vec3 aNormal;" \
		"uniform mat4 uModelMatrix;" \
		"uniform mat4 uViewMatrix;" \
		"uniform mat4 uProjectionMatrix;" \
		"uniform vec3 ulightAmbient[2];" \
		"uniform vec3 ulightDiffuse[2];" \
		"uniform vec3 ulightSpecular[2];" \
		"uniform vec4 uLightPosition[2];" \
		"uniform vec3 umaterialAmbient;" \
		"uniform vec3 umaterialDiffuse;" \
		"uniform vec3 umaterialSpecular;" \
		"uniform float umaterialShininess;" \
		"uniform int ukeypressed;" \
		"out vec3 ophong_ads_light;" \
		"in vec4 aColor;" \
		"out vec4 oColor;" \
		"void main(void)" \
		"{" \
		"if(ukeypressed == 1)" \
		"{" \
		"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;" \
		"vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);" \
		"vec3 lightDirection[2];" \
		"vec3 reflectionVector[2];" \
		"vec3 viewerVector = normalize(-eyeCoordinates.xyz);" \
		"vec3 ambientLight[2];" \
		"vec3 diffuseLight[2];" \
		"vec3 specularLight[2];" \
		"for(int i = 0; i<2; i++)" \
		"{" \
		"ambientLight[i] = ulightAmbient[i] +umaterialAmbient;" \
		"lightDirection[i] = normalize(vec3(uLightPosition[i] - eyeCoordinates));" \
		"diffuseLight[i] =ulightDiffuse[i] * umaterialDiffuse * max(dot(lightDirection[i], transformedNormals), 0.0f);" \
		"reflectionVector[i] = reflect(-lightDirection[i], transformedNormals);" \
		"vec3 viewerVector = normalize(-eyeCoordinates.xyz);" \
		"specularLight[i] = ulightSpecular[i] * umaterialSpecular * pow(max(dot(reflectionVector[i], viewerVector), 0.0f), umaterialShininess);" \
		"ophong_ads_light = ophong_ads_light + ambientLight[i] + diffuseLight[i] + specularLight[i];" \
		"}" \
		"}" \
		"else" \
		"{" \
		"ophong_ads_light = vec3(0.0f , 0.0f , 0.0f);" \
		"}" \
		"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;" \
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
		uninitialized();
	}

	// fragment shader

	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 oColor;" \
		"in vec3 ophong_ads_light;" \
		"out vec4 FragColor;" \
		"uniform int ukeypressed;" \
		"void main(void)" \
		"{" \
		"if(ukeypressed == 1)" \
		"{" \
		"FragColor = vec4(ophong_ads_light, 1.0);" \
		"}" \
		"else" \
		"{" \
		"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0);" \
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
		uninitialized();
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
		uninitialized();
	}

	// get shader uniform location
	ModelMatrixUniform = glGetUniformLocation(shaderProgramObject, "uModelMatrix");
	ViewMatrixUniform = glGetUniformLocation(shaderProgramObject, "uViewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");

	lightAmbientUniform[0] = glGetUniformLocation(shaderProgramObject, "ulightAmbient[0]");
	lightDiffuseUniform[0] = glGetUniformLocation(shaderProgramObject, "ulightDiffuse[0]");
	lightSpecularUniform[0] = glGetUniformLocation(shaderProgramObject, "ulightSpecular[0]");
	lightPositionUniform[0] = glGetUniformLocation(shaderProgramObject, "uLightPosition[0]");

	lightAmbientUniform[1] = glGetUniformLocation(shaderProgramObject, "ulightAmbient[1]");
	lightDiffuseUniform[1] = glGetUniformLocation(shaderProgramObject, "ulightDiffuse[1]");
	lightSpecularUniform[1] = glGetUniformLocation(shaderProgramObject, "ulightSpecular[1]");
	lightPositionUniform[1] = glGetUniformLocation(shaderProgramObject, "uLightPosition[1]");


	materialAmbientUniform = glGetUniformLocation(shaderProgramObject, "umaterialAmbient");
	materialDiffuseUniform = glGetUniformLocation(shaderProgramObject, "umaterialDiffuse");
	materialSpecularUniform = glGetUniformLocation(shaderProgramObject, "umaterialSpecular");
	materialShininessUniform = glGetUniformLocation(shaderProgramObject, "umaterialShininess");


	keyPressUniform = glGetUniformLocation(shaderProgramObject, "ukeypressed");

	// declare position and color arrays

	const GLfloat Pyramide_position[] =
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

	GLfloat pyramidNormals[] =
	{
		// front
		0.000000f, 0.447214f,  0.894427f, // front-top
		0.000000f, 0.447214f,  0.894427f, // front-left
		0.000000f, 0.447214f,  0.894427f, // front-right

		// right			    
		0.894427f, 0.447214f,  0.000000f, // right-top
		0.894427f, 0.447214f,  0.000000f, // right-left
		0.894427f, 0.447214f,  0.000000f, // right-right

		// back
		0.000000f, 0.447214f, -0.894427f, // back-top
		0.000000f, 0.447214f, -0.894427f, // back-left
		0.000000f, 0.447214f, -0.894427f, // back-right

		// left
	   -0.894427f, 0.447214f,  0.000000f, // left-top
	   -0.894427f, 0.447214f,  0.000000f, // left-left
	   -0.894427f, 0.447214f,  0.000000f, // left-right
	};


	// Pyramid 
	{
		// VAO = vertex array object

		glGenVertexArrays(1, &vao_Pyramid);

		glBindVertexArray(vao_Pyramid);

		// VBO for position
		glGenBuffers(1, &vbo_position_Pyramid);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Pyramid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramide_position), Pyramide_position, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// VBO for normal

		glGenBuffers(1, &vbo_normal_Pyramid);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_Pyramid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMALS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		// unbind with vao
		glBindVertexArray(0);
	}



	// enabling depth 

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	light[0].ambient = vec3{ 0.0f, 0.0f, 0.0f };
	light[1].ambient = vec3{ 0.0f, 0.0f, 0.0f };

	light[0].diffuse = vec3{ 1.0f, 0.0f, 0.0f };
	light[1].diffuse = vec3{ 0.0f, 0.0f, 1.0f };

	light[0].specular = vec3{ 1.0f, 0.0f, 0.0f };
	light[1].specular = vec3{ 0.0f, 0.0f, 1.0f };

	light[0].position = vec4{-2.0f, 0.0f, 0.0f, 1.0f};
	light[1].position = vec4{ 2.0f, 0.0f, 0.0f, 1.0f };

	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Here OpenGL starts.....

	perspectiveProjectionMatrix = vmath::mat4::identity();


	resize(WIN_WIDTH, WIN_HEIGHT);

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

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);

}

void display(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObject);

	// triangle
	{
		// transformation

		mat4 modelMatrix = mat4::identity();
		mat4 ViewMatrix = mat4::identity();

		mat4 translationMatrix = mat4::identity();
		translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);

		mat4 rotationMatrix = mat4::identity();
		rotationMatrix = vmath::rotate(angle_Pyramid, 0.0f, 1.0f, 0.0f);


		modelMatrix = translationMatrix * rotationMatrix;

		// now push modelViewProjectionMatrix in the "uMVPMatrix" of shader , per frame.

		// push above mvp into vertex shader mvpMatrixUniform
		glUniformMatrix4fv(ModelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewMatrixUniform, 1, GL_FALSE, ViewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
		// for light

		if (bLightingEnabled == TRUE)
		{
			glUniform1i(keyPressUniform, 1);
			glUniform3fv(lightAmbientUniform[0], 1, light[0].ambient);
			glUniform3fv(lightDiffuseUniform[0], 1, light[0].diffuse);
			glUniform3fv(lightSpecularUniform[0], 1, light[0].specular);

			glUniform3fv(lightAmbientUniform[1], 1, light[1].ambient);
			glUniform3fv(lightDiffuseUniform[1], 1, light[1].diffuse);
			glUniform3fv(lightSpecularUniform[1], 1, light[1].specular);


			glUniform3fv(materialAmbientUniform, 1, materialAmbient);
			glUniform3fv(materialDiffuseUniform, 1, materialDiffuse);
			glUniform3fv(materialSpecularUniform, 1, materialSpecular);
			glUniform1f(materialShininessUniform, materialShininess);


			glUniform4fv(lightPositionUniform[0], 1, light[0].position);
			glUniform4fv(lightPositionUniform[1], 1, light[1].position);
		}
		else
		{
			glUniform1i(keyPressUniform, 0);
		}


		glBindVertexArray(vao_Pyramid);
		glDrawArrays(GL_TRIANGLES, 0, 12);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}


	glUseProgram(0);

	SwapBuffers(ghdc);

}
 
void update(void)
{

	angle_Pyramid -= 0.5f;
	if (angle_Pyramid <= 0.0f)
	{
		angle_Pyramid += 360.0f;
	}

}

void uninitialized(void)
{
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

	// for triangle 
	{
		// delete VBO of color

		if (vbo_color_Pyramid)
		{
			glDeleteBuffers(1, &vbo_color_Pyramid);
			vbo_color_Pyramid = 0;
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

	void ToggleFullscreen(void);
	if (gbFullscreen == TRUE)
	{
		ToggleFullscreen();
		gbFullscreen = FALSE;
	}

	// make the hdc as current context

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}
	
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;

	}

	// release the hdc

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
	}

	// code
	// if application is exiting in fullscreen
	// destroy window
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	// close the log file

	if (gpFile)
	{
		fprintf(gpFile, "Program ended successfully \n");
		fclose(gpFile);
		gpFile = NULL;
	}
}




