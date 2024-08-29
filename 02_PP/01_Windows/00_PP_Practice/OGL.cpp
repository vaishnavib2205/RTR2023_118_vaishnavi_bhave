//*********************by default command prompt compiles and runs in release mode 
//by default vscode runs in debug mode**********************************

#include<Windows.h> // Windows header files
#include "OGL.h"
#include<stdlib.h> // For exit
#include<stdio.h> // For file IO (input output)

// OpenGL header files
// pp madhe glu nahi
#include<gl/glew.h> //this must be before gl/GL.h
#include<gl/GL.h>
#include"vmath.h" //our own math file
using namespace vmath;


// MACROS

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Link with OpenGL library
#pragma comment(lib, "glew32.lib")
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

GLuint shaderProgramObject = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR 

};

GLuint vao_Triangle = 0;
GLuint vbo_position_triangle = 0;
GLuint vbo_color_triangle = 0;

GLuint vao_Square = 0;
GLuint vbo_position_square = 0;
GLuint vbo_color_square = 0;

GLuint mvpMatrixUniform = 0;

mat4 perspectiveProjectionMatrix = 0;
//mat4 -> 4 by 4 cha matrix -> is in vmath.h


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

	if(fopen_s(&gpFile, "log.txt", "w") !=0)
	{
		MessageBox(NULL, TEXT("Log file cannot be open!!!"), TEXT("File create error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "Program started successfully \n");
	fprintf(gpFile, "____________________________ \n");

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
		TEXT("Vaishnavi_cha_PP_cha_Shri_Ganesha!!"),
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
	
	//initialise GLEW 
	// ****** importnt***

	if (glewInit() != GLEW_OK)
	{
		fprintf(gpFile, "glewInit() failed to initialise \n");
		return -6;
	}

	//print glInfo
	printGLInfo();


	// Vertex Shader

	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 aPosition;" \
		"in vec4 aColor;" \
		"out vec4 oColor;" \
		"uniform mat4 uMVPMatrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = uMVPMatrix * aPosition;" \
		"oColor = aColor;" \
		"}";


		
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	// let openGL compile vertex shader
	glCompileShader(vertexShaderObject);

	//**************STEPS FOR INITIALISE FUNC*************************


	//check for vertex shader compilation errors if any
	// a. get compile status
	// b. if compilation error get length of the compilation info log
	// c. declare a string variabke and allocate memory of above length
	// d. get the compilation error info log into memory allocated string 
	// e. print compilation error info log
	// f. free the allocated memory 
	// g. uninitialise and exit

	GLint status = 0;
	GLint infoLogLength = 0;
	GLchar* szInfoLog = NULL;


	//length
		glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) // if error
		{
			glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);

				if (szInfoLog != NULL)
				{
					glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "VertexShader Compilation Error Log : %s\n", szInfoLog);

					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized();
		}


		// FRAGMENT SHADER

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


		//for enabling shader 
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
					fprintf(gpFile, "Fragment Shader Compilation Log: \n", szInfoLog);
					free(szInfoLog);
					szInfoLog =NULL;
				}
			}
			uninitialized();
		}

		//shader program
		// create shader program object
		shaderProgramObject = glCreateProgram();

		// attach both shader to this program
		glAttachShader(shaderProgramObject, vertexShaderObject);
		glAttachShader(shaderProgramObject, fragmentShaderObject);

		//bind Attribute location with the shader program object

		glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");

		glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "aColor");

		//link the shader program
		glLinkProgram(shaderProgramObject);

		//do above shader compilation related steps for linking error 

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
					fprintf(gpFile, "Shader Program Linking Error Log: %s\n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;					
				}
			}
			uninitialized();
		}

		//get shader uniform locations
		mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");


		//SQUARE

		//declare position and color arrays
		const GLfloat square_position[] =
		{ 
			1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,

			1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,

			0.1f, 0.62f, 0.0f,
			-0.1f, 0.612f, 0.0f,
			-0.17f, 0.56f, 0.0f,
			0.14f, 0.59f, 0.0f,

			0.08f, 0.29f, 0.0f,
			-0.2f, 0.42f, 0.0f,
			-0.35f, 0.1f, 0.0f,
			-0.14f, -0.06f, 0.0f,

			0.24f, 0.22f, 0.0f,
			0.09f, 0.23f, 0.0f,
			0.03f, 0.13f, 0.0f,
			0.11f, 0.1f, 0.0f,

			0.11f, 0.1f, 0.0f,
			0.03f, 0.13f, 0.0f,
			0.0f, 0.09f, 0.0f,
			0.08f, -0.11f, 0.0f,

			-0.41f, 0.0f, 0.0f,
			-0.6f, -0.18f, 0.0f,
			-0.53f, -0.4f, 0.0f,
			-0.29f, -0.3f, 0.0f,

			-0.39f, -0.4f, 0.0f,
			-0.44f, -0.58f, 0.0f,
			-0.10f, -1.0f, 0.0f,
			-0.15f, -0.56, 0.0f,

			0.1f, -0.43f, 0.0f,
			-0.09f, -0.97f, 0.0f,
			0.33f, -0.57f, 0.0f,
			0.36f, -0.38, 0.0f


		
		};

		const GLfloat square_color[] =
		{
			0.93f, 0.81f, 0.92f,
			0.93f, 0.81f, 0.92f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,

			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			0.54f, 0.83f, 0.95f,
			0.54f, 0.83f, 0.95f,

			1.0f, 0.64f, 0.0f,
			1.0f, 0.64f, 0.0f,
			1.0f, 0.64f, 0.0f,
			1.0f, 0.64f, 0.0f,

			1.0f, 0.373f, 0.00f,
			1.0f, 0.373f, 0.00f,
			1.0f, 0.373f, 0.00f,
			1.0f, 0.373f, 0.00f,

			1.0f, 0.443f, 0.003f,
			1.0f, 0.443f, 0.003f,
			1.0f, 0.443f, 0.003f,
			1.0f, 0.443f, 0.003f,

			1.0f, 0.443f, 0.003f,
			1.0f, 0.443f, 0.003f,
			1.0f, 0.443f, 0.003f,
			1.0f, 0.443f, 0.003f,

			1.0f, 0.509f, 0.003f,
			1.0f, 0.509f, 0.003f,
			1.0f, 0.509f, 0.003f,
			1.0f, 0.509f, 0.003f,

			1.0f, 0.58f, 0.003f,
			1.0f, 0.58f, 0.003f,
			1.0f, 0.58f, 0.003f,
			1.0f, 0.58f, 0.003f,

			1.0f, 0.58f, 0.003f,
			1.0f, 0.58f, 0.003f,
			1.0f, 0.58f, 0.003f,
			1.0f, 0.58f, 0.003f



		};

		//TRIANGLE
		const GLfloat triangle_position[] =
		{
			0.08f, 0.65f, 0.0f,
			0.0f, 0.8f, 0.0f,
			-0.08f, 0.65f, 0.0f,

			0.12f, 0.56f, 0.0f,
			-0.2f, 0.5f, 0.0f,
			0.1f, 0.48f, 0.0f,

			0.1f, 0.45f, 0.0f,
			-0.22f, 0.45f, 0.0f,
			0.08f, 0.32f, 0.0f,

			-0.14f, -0.06f, 0.0f,
			-0.35f, 0.1f, 0.0f,
			0.085f, -0.52f, 0.0f,

			-0.25f, 0.41f, 0.0f,
			-0.38f, 0.55f, 0.0f,
			-0.40f, 0.12f, 0.0f,

			0.39f, -0.2f, 0.0f,
			0.19f, -0.1f, 0.0f,
			0.11f, -0.21f, 0.0f,

			0.39f, -0.23f, 0.0f,
			0.19f, -0.36f, 0.0f,
			0.11f, -0.26f, 0.0f,

			0.4f, -0.82f, 0.0f,
			0.3f, -0.92f, 0.0f,
			0.45f, -0.92f, 0.0f,

			0.72f, -0.92f, 0.0f,
			0.5f, -0.82f, 0.0f,
			0.43f, -0.92f, 0.0f

		};

		const GLfloat triangle_color[] =
		{

			1.0f, 0.64f, 0.0f,
			1.0f, 0.64f, 0.0f,
			1.0f, 0.64f, 0.0f,

			1.0f, 0.64f, 0.0f,
			1.0f, 0.64f, 0.0f,
			1.0f, 0.64f, 0.0f,

			1.0f, 0.64f, 0.0f,
			1.0f, 0.64f, 0.0f,
			1.0f, 0.64f, 0.0f,

			1.0f, 0.373f, 0.00f,
			1.0f, 0.373f, 0.00f,
			1.0f, 0.373f, 0.00f,

			1.0f, 0.443f, 0.003f,
			1.0f, 0.443f, 0.003f,
			1.0f, 0.443f, 0.003f,

			1.0f, 0.64f, 0.0f,
			1.0f, 0.64f, 0.0f,
			1.0f, 0.64f, 0.0f,

			1.0f, 0.509f, 0.003f,
			1.0f, 0.509f, 0.003f,
			1.0f, 0.509f, 0.003f,

			0.458f, 0.447f, 0.435f,
			0.458f, 0.447f, 0.435f,
			0.458f, 0.447f, 0.435f,

			0.502f, 0.486f, 0.470f,
			0.502f, 0.486f, 0.470f,
			0.502f, 0.486f, 0.470f


		};



		//const GLfloat square_color[] =
		//{
		//	1.0f, 0.0f, 0.0f, //glColor3f(1.0f, 0.0f, 0.0f);
		//	0.0f, 1.0f, 0.0f, // glColor3f(0.0f, 1.0f, 0.0f); 
		//	0.0f, 0.0f, 1.0f  //glColor3f(0.0f, 0.0f, 1.0f); 

		//}; // square la single color aahe tyamule array ne karychi garaj nahi vbo aani color array lagnar nai


		//*********** TRIANGLE ***********
		
		//VAO
		//create vertex array object
		glGenVertexArrays(1, &vao_Triangle);

		//bind with vao
		glBindVertexArray(vao_Triangle);

		// VBO for position
		//create vertex buffer array for position
		glGenBuffers(1, &vbo_position_triangle);

		//bind with vbo of position
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position_triangle);

		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_position), triangle_position, GL_STATIC_DRAW);

		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

		//unbind with vbo of positin
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//REPEAT vbo position steps for vbo color
		//vbo for color
		glGenBuffers(1, &vbo_color_triangle);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color_triangle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_color), triangle_color, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

		//UNBIND WITH VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		//SQUARE
		//VAO
		//create vertex array object
		glGenVertexArrays(1, &vao_Square);

		//bind with vao
		glBindVertexArray(vao_Square);

		// VBO for position
		//create vertex buffer array for position
		glGenBuffers(1, &vbo_position_square);

		//bind with vbo of position
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);

		glBufferData(GL_ARRAY_BUFFER, sizeof(square_position), square_position, GL_STATIC_DRAW);

		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

		
		//unbind with vbo of positin
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &vbo_color_square);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color_square);
		glBufferData(GL_ARRAY_BUFFER, sizeof(square_color), square_color, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

		//UNBIND WITH VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);


	// enabling depth tests
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// set the clear colour of window to blue

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//initialize orthographic projection matrix
	perspectiveProjectionMatrix = vmath::mat4::identity();


	// Here OpenGL starts.....
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void printGLInfo(void)
{
	//variable declaration 
	GLint numextensions;
	GLint i;
	//listing of supported extensions

	fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer :%s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "OpenGL GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(gpFile, "______________________________________________________________________________________ \n");

	glGetIntegerv(GL_NUM_EXTENSIONS, &numextensions);
	fprintf(gpFile, "Number Of EXT : %d\n", numextensions);
	for (i = 0; i < numextensions; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}
	fprintf(gpFile, "______________________________________________________________________________________ \n");
}

void resize(int width, int height)
{
	// code
	if (height <= 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//set orthographic projection matrix
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);

}

void display(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ***********************STEPS FOR DISPLAY**************************

	//1. use shader prog object 
	// 2. bind with vao

	glUseProgram(shaderProgramObject);
	
	//Triangle
	mat4 modelViewMatrix = mat4::identity();
	modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; //matrix mdhe multiplication la commutitive property that is a*b != b*a in matrices

	

	//push above mvp into vertex shader's mvpUniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	

	glBindVertexArray(vao_Square);
	glDrawArrays(GL_QUADS, 0, 36);
	glBindVertexArray(0);

	glBindVertexArray(vao_Triangle);
	glDrawArrays(GL_TRIANGLES, 0, 27);
	glBindVertexArray(0);

	glUseProgram(0);
	SwapBuffers(ghdc);

}
 
void update(void)
{
	// Code

}

void uninitialized(void)
{
	fprintf(gpFile, "__________________________\n");
	// *****************STEPS FOR SHADER PROG AND OBJ UNINITIALISATION*************************
	// 1. use shader Program object
	// 2. get number of attached shaders 
	// 3. Detach and delete shader objects iteratively or in loop
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);

		GLint numShaders = 0;

		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);

		// create and allocate array of shader object according to the number of attached shaders
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));

			//get shader objects into this allocated array

			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);

			}
			for (GLint i = 0; i < numShaders; i++)
			{
				glDetachShader(shaderProgramObject, pShaders[i]);
				glDeleteShader(pShaders[i]);
				pShaders[i] = 0;
			}

			free(pShaders);
			pShaders = NULL;
		}
		//unuse
		glUseProgram(0);
		// del shader prog object
		glDeleteProgram(shaderProgramObject);

		shaderProgramObject = 0;

	}

	//delete vbo for color and position
	// delete vao 

	if (vbo_color_triangle)
	{
		glDeleteBuffers(1, &vbo_color_triangle);
		vbo_color_triangle = 0;
	}

	//delete vbo pos
	if (vbo_position_triangle)
	{
		glDeleteBuffers(1, &vbo_position_triangle);
		vbo_position_triangle = 0;
	}

	if (vao_Triangle)
	{
		glDeleteVertexArrays(1, &vao_Triangle);
		vao_Triangle = 0;
	}


	//delete vbo pos
	if (vbo_position_square)
	{
		glDeleteBuffers(1, &vbo_position_square);
		vbo_position_square = 0;
	}

	if (vao_Square)
	{
		glDeleteVertexArrays(1, &vao_Square);
		vao_Square = 0;
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



