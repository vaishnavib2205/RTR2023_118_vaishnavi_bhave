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
	AMC_ATTRIBUTE_COLOR

};

GLuint vao_triangle = 0;
GLuint vbo_position_triangle = 0;

GLuint vao_square = 0;
GLuint vbo_position_square = 0;

GLuint vao_circle = 0;
GLuint vbo_position_circle = 0;
GLuint vbo_color = 0;

GLuint vao_blueLine = 0;
GLuint vbo_position_blueLines = 0;

GLuint vao_horizontal_blueLines = 0;
GLuint vbo_position_HorizontalBlueLines= 0;
GLuint vbo_blue_color = 0;

GLuint vao_greenLine = 0;
GLuint vbo_position_greenLine = 0;
GLuint vbo_greenColor = 0;

GLuint vao_redLine = 0;
GLuint vbo_position_redLine = 0;
GLuint vbo_redColor = 0;

GLuint mvpMatrixUniform = 0;

mat4 perspectiveProjectionMatrix; // mat4 datatype is in vmath.h header file, that means 4X4 matrix

GLboolean Graph = FALSE;
GLboolean Triangle = FALSE;
GLboolean Square = FALSE;
GLboolean CirclePressed = FALSE;

float x = 0.0f;
float y = 0.0f;
float Radian;

int i = 0;
GLfloat circle_position[1081];

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
	TCHAR szAppName[] = TEXT("VBWindow");
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
		TEXT("Vaishnavi Bhave"),
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

				// Update
				update();

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
		case 'G':
		case 'g':

			if (Graph == FALSE)
			{
				Graph = TRUE;
			}
			else
			{
				Graph = FALSE;
			}
			
			break;

		case 'T':
		case 't':
			if (Triangle == FALSE)
			{
				Triangle = TRUE;
			}
			else
			{
				Triangle = FALSE;
			}
			break;

		case 'S':
		case 's':
			if (Square == FALSE)
			{
				Square = TRUE;
			}
			else
			{
				Square = FALSE;
			}

			break;

		case 'C':
		case 'c':

			if (CirclePressed == FALSE)
			{
				CirclePressed = TRUE;
			}
			else
			{
				CirclePressed = FALSE;
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
		"uniform mat4 uMVPMatrix;" \
		"in vec4 aColor;" \
		"out vec4 oColor;" \
		"void main(void)" \
		"{" \
		"gl_Position = uMVPMatrix * aPosition;" \
		"oColor = aColor;" \
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
		uninitialized();
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
		uninitialized();
	}

	// get shader uniform location

	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");

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
	glGenVertexArrays(1, &vao_blueLine);
	glBindVertexArray(vao_blueLine);

	// verticle VBO for position
	glGenBuffers(1, &vbo_position_blueLines);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_blueLines);
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
	glGenVertexArrays(1, &vao_horizontal_blueLines);
	glBindVertexArray(vao_horizontal_blueLines);

	// horizontal VBO for position
	glGenBuffers(1, &vbo_position_HorizontalBlueLines);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_HorizontalBlueLines);
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
	glGenVertexArrays(1, &vao_redLine);
	glBindVertexArray(vao_redLine);

	// verticle VBO for position
	glGenBuffers(1, &vbo_position_redLine);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_redLine);
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
	glGenBuffers(1, &vbo_redColor);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_redColor);
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
	glGenVertexArrays(1, &vao_greenLine);
	glBindVertexArray(vao_greenLine);

	// verticle VBO for position
	glGenBuffers(1, &vbo_position_greenLine);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_greenLine);
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
	glGenBuffers(1, &vbo_greenColor);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_greenColor);
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


	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
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

	mat4 modelViewMatrix = mat4::identity();
	modelViewMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
	// Order of multiplication vimp
	mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	if(Graph == TRUE)
	{
		for (float i = 0.1f; i < 4.0f; i += 0.1)
		{
			modelViewMatrix = mat4::identity();
			modelViewMatrix = vmath::translate(i, 0.0f, -6.0f);
			// Order of multiplication vimp
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

			glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

			glBindVertexArray(vao_blueLine);
			glDrawArrays(GL_LINES, 0, 2);
			glBindVertexArray(0);
		}
		for (float i = -0.1f; i > -4.0f; i -= 0.1)
		{
			modelViewMatrix = mat4::identity();
			modelViewMatrix = vmath::translate(i, 0.0f, -6.0f);
			// Order of multiplication vimp
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

			glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

			glBindVertexArray(vao_blueLine);
			glDrawArrays(GL_LINES, 0, 2);
			glBindVertexArray(0);
		}
		for (float i = -0.1f; i > -4.0f; i -= 0.1)
		{
			modelViewMatrix = mat4::identity();
			modelViewMatrix = vmath::translate(0.0f, i, -6.0f);
			// Order of multiplication vimp
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

			glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

			glBindVertexArray(vao_horizontal_blueLines);
			glDrawArrays(GL_LINES, 0, 2);
			glBindVertexArray(0);
		}
		for (float i = 0.1f; i < 4.0f; i += 0.1)
		{
			modelViewMatrix = mat4::identity();
			modelViewMatrix = vmath::translate(0.0f, i, -6.0f);
			// Order of multiplication vimp
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

			glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

			glBindVertexArray(vao_horizontal_blueLines);
			glDrawArrays(GL_LINES, 0, 2);
			glBindVertexArray(0);
		}

		modelViewMatrix = mat4::identity();
		modelViewMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
		// Order of multiplication vimp
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		glBindVertexArray(vao_redLine);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);


		modelViewMatrix = mat4::identity();
		modelViewMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
		// Order of multiplication vimp
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		glBindVertexArray(vao_greenLine);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);

	}
	if(Triangle == TRUE)
	{
		glBindVertexArray(vao_triangle);
		glDrawArrays(GL_LINES, 0, 6);
		glBindVertexArray(0);
	}
	if(Square == TRUE)
	{
		glBindVertexArray(vao_square);
		glDrawArrays(GL_LINES, 0, 8);
		glBindVertexArray(0);
	}
	if(CirclePressed == TRUE)
	{
		glBindVertexArray(vao_circle);

		for (int i = 0; i < 119; i++) {

			glDrawArrays(GL_LINES, i, 242);

		}

		glBindVertexArray(0);
	}

	glUseProgram(0);

	SwapBuffers(ghdc);

}
 
void update(void)
{
	// Code

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



