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
//namespace is like a structure


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
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_TEXCOORD

};

GLuint vao = 0;
GLuint vbo_position = 0;
GLuint vbo_color = 0;
GLuint vbo_texcoord = 0;

GLuint mvpMatrixUniform = 0;
GLuint textureSamplerUniform = 0;
GLuint keypressUniform = 0;

GLuint texture_smiley = 0;

int key_used = 0;

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
		TEXT("ho_mazich_First_PP_BlueWindow"),
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

		case VK_NUMPAD1:
		case 1:
			key_used = 1;
			glEnable(GL_TEXTURE_2D);
			break;

		case VK_NUMPAD2:
		case 2:
			key_used = 2;
			glEnable(GL_TEXTURE_2D);
			break;

		case VK_NUMPAD3:
		case 3:
			key_used = 3;
			glEnable(GL_TEXTURE_2D);
			break;

		case VK_NUMPAD4:
		case 4:
			key_used = 4;
			glEnable(GL_TEXTURE_2D);
			break;

		default:
			glDisable(GL_TEXTURE_2D);
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
	BOOL loadGlTexture(GLuint*, TCHAR[]);

	BOOL bResult;
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
		"in vec2 aTexCoord;" \
		"out vec2 oTexCoord;" \
		"uniform mat4 uMVPMatrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = uMVPMatrix * aPosition;" \
		"oTexCoord = aTexCoord;" \
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
		// when you use data type as functions called as constructor vec4
		const GLchar* fragmentShaderSourceCode =
			"#version 460 core" \
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

		glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");

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

		textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "uTextureSampler");

		keypressUniform = glGetUniformLocation(shaderProgramObject, "ukey_used");

		//declare position and color arrays
		const GLfloat rectangle_position[] =
		{
			1.0f, 1.0f, 0.0f ,		//glVertex3f(1.0f, 1.0f, 0.0f);
			-1.0f, 1.0f, 0.0f ,		//glVertex3f(-1.0f, 1.0f, 0.0f);
			-1.0f, -1.0f, 0.0f ,	//glVertex3f(-1.0f, -1.0f, 0.0f);
			1.0f, -1.0f, 0.0f		//glVertex3f(1.0f, -1.0f, 0.0f);
		};

		//VAO
		//create vertex array object
		glGenVertexArrays(1, &vao);

		//bind with vao
		glBindVertexArray(vao);

		// VBO for position
		//create vertex buffer array for position
		glGenBuffers(1, &vbo_position);

		//bind with vbo of position
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);

		glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_position), rectangle_position, GL_STATIC_DRAW);

		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

		//unbind with vbo of positin
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//VBO for texcoord
		glGenBuffers(1, &vbo_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
		glBufferData(GL_ARRAY_BUFFER, 4*2*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//UNBIND WITH VAO
		glBindVertexArray(0);

		bResult = loadGlTexture(&texture_smiley, MAKEINTRESOURCE(MY_SMILE_BITMAP));
		if (bResult == FALSE)
		{
			fprintf(gpFile, "loading of smiley texture failed \n");
			return -6;
		}

		// tell OpenGL to enable the TEXTURE

		glEnable(GL_TEXTURE_2D);  // 4th step
		//initialize orthographic projection matrix

	// enabling depth tests
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Here OpenGL starts.....
	

	
	perspectiveProjectionMatrix = vmath::mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);
	return 0;
}

BOOL loadGlTexture(GLuint* texture, TCHAR imageResourceID[])
{
	// local variable declarations
	HBITMAP hBitmap = NULL;
	BITMAP bmp;

	// load the image , 5th step

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap == NULL)
	{
		fprintf(gpFile, "loading image failed \n");
		return FALSE;
	}

	// Get image data 6th step

	//GetObject(hBitmap, sizeof(BITMAP), bmp);
	GetObject(hBitmap, sizeof(BITMAP), &bmp);

	glGenTextures(1, texture);

	// bind to the generated texture

	glBindTexture(GL_TEXTURE_2D, *texture);

	// 9th step

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// set texture paramerter

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// 11th step create multiple mipmap images
	
	/*gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, (void*)bmp.bmBits);*/

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, (void*)bmp.bmBits);

	glGenerateMipmap(GL_TEXTURE_2D);

	// 12th step
	glBindTexture(GL_TEXTURE_2D, 0);

	DeleteObject(hBitmap);

	hBitmap = NULL;

	return TRUE;

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
	//VAR DECLARATION
	GLfloat square_texcoord[8];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ***********************STEPS FOR DISPLAY**************************

	//1. use shader prog object 
	// 2. bind with vao

	glUseProgram(shaderProgramObject);

	//Transformation
	mat4 modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; //matrix mdhe multiplication la commutitive property that is a*b != b*a in matrices

	//push above mvp into vertex shader's mvpUniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//for texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_smiley);
	glUniform1i(textureSamplerUniform, 0);
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

	//draw geometry / model / scenes
	glDrawArrays(GL_QUADS, 0, 4);
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

	if (vbo_texcoord)
	{
		glDeleteBuffers(1, &vbo_texcoord);
		vbo_texcoord = 0;
	}

	//delete vbo pos
	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
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

	if (texture_smiley)
	{
		glDeleteTextures(1, &texture_smiley);
		texture_smiley = 0;
	}
	// close the log file

	if (gpFile)
	{
		fprintf(gpFile, "Program ended successfully \n");
		fclose(gpFile);
		gpFile = NULL;
	}
}



