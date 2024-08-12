//by default command prompt compiles and runs in release mode 
//by default vscode runs in debug mode

#include<Windows.h> // Windows header files
#include "OGL.h"
#include<stdlib.h> // For exit
#include<stdio.h> // For file IO (input output)

// OpenGL header files
// pp madhe glu nahi
#include<gl/glew.h> //this must be before gl/GL.h
#include<gl/GL.h>

#include"vmath.h" //our own math file
#include "Sphere.h"

using namespace vmath;

// MACROS

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Link with OpenGL library
#pragma comment(lib, "glew32.lib")
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"Sphere.lib")

//global function delaration



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Entry point function

// global variable declaration
FILE* gpFile = NULL;

HWND ghwnd = NULL;
BOOL gbActive = False;

DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
BOOL gbFullscreen = False;

//OpenGL related Global variables
HDC ghdc = NULL;   // DC
HGLRC ghrc = NULL; // rendating context

GLuint shaderProgramObject_PV = 0;

GLuint shaderProgramObject_PF = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORDS

};

//variables/arrays related to sphere

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

GLuint gVao_sphere = 0;
GLuint gVbo_sphere_position = 0;
GLuint gVbo_sphere_normal = 0;
GLuint gVbo_sphere_element = 0;
GLuint vbo_texcoords = 0;

GLuint gNumVertices;
GLuint gNumElements;

GLuint ModelMatrixUniform_PV = 0;
GLuint ViewMatrixUniform_PV = 0;
GLuint projectionMatrixUniform_PV = 0;

GLuint lightDiffuseUniform_PV = 0;
GLuint lightAmbientUniform_PV = 0;
GLuint lightSpecularUniform_PV = 0;
GLuint lightPositionUniform_PV = 0;

GLuint materialDiffuseUniform_PV = 0;
GLuint materialAmbientUniform_PV = 0;
GLuint materialSpecularUniform_PV = 0;
GLuint materialShininessUniform_PV = 0;

GLuint keypressedUniform = 0;

GLuint ModelMatrixUniform_PF = 0;
GLuint ViewMatrixUniform_PF = 0;
GLuint projectionMatrixUniform_PF = 0;

GLuint lightDiffuseUniform_PF = 0;
GLuint lightAmbientUniform_PF = 0;
GLuint lightSpecularUniform_PF = 0;
GLuint lightPositionUniform_PF = 0;

GLuint materialDiffuseUniform_PF = 0;
GLuint materialAmbientUniform_PF = 0;
GLuint materialSpecularUniform_PF = 0;
GLuint materialShininessUniform_PF = 0;

BOOL bLightingEnabled = False;
BOOL bAnimationEnabled = False;


BOOL bLightEnabled_PV = False;
BOOL bLightEnabled_PF = False;

GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // color intensity
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat materialDiffuse[] = { 0.5f, 0.2f, 0.7f, 1.0f };
GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat materialShininess =  128.0f ;


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
	BOOL bDone = False;

	if (fopen_s(&gpFile, "log.txt", "w") != 0)
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
	while (bDone == False)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = True;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActive == True)
			{
				// Render
				display();

				if (bAnimationEnabled == True)
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
		gbActive = True;
		break;

	case WM_KILLFOCUS:
		gbActive = False;
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
			if (gbFullscreen == False)
			{
				ToggleFullscreen();
				gbFullscreen = True;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = False;
			}
			break;
		}
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'Q':
		case 'q':
			DestroyWindow(hwnd);
			break;

		case 'L':
		case 'l':
			if (bLightingEnabled == False)
			{
				bLightingEnabled = True;
			}
			else
			{
				bLightingEnabled = False;
			}
			break;

		case 'V':
		case 'v':
			if (bLightEnabled_PV == False)
			{
				bLightEnabled_PV = True;
			}
			else
			{
				bLightEnabled_PV = False;
			}
			break;

		case 'F':
		case 'f':
			if (bLightEnabled_PF == False)
			{

				bLightEnabled_PF = True;
			}
			else
			{
				bLightEnabled_PF = False;
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
	if (gbFullscreen == False)
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
		ShowCursor(False);
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
		ShowCursor(True);
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

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == False)
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

	if (wglMakeCurrent(ghdc, ghrc) == False)
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

	//mathematical steps for diffuse light
	//diffuse light = ld * kd * (s n);
	//step 1: calculate i coords by multiplying modelviewMatrix and position attribute vectors
	//step 2: calculate normal matrix by taking 3 * 3 matrix from upper view of model view matrix by inverting and transposing 
	//step 3: Transform normals by normalising the multiplication of normal matrix and normal attributes
	//step 4: calculate light source vector by normalising the subtraction of lightposition vector and iPosition vector 
	// step 5: finally the diffuse light will be ld * kd * (s.n);

	// ****************** PER VERTEX LIGHT ********************************

	{

		const GLchar* vertexShaderSourceCode_PV =
			"#version 460 core" \
			"\n" \
			"in vec4 aPosition;" \
			"in vec3 aNormal;" \
			"uniform mat4 uModelMatrix;" \
			"uniform mat4 uViewMatrix;" \
			"uniform mat4 uProjectionMatrix;" \
			"uniform vec3 ulightAmbient;" \
			"uniform vec3 ulightDiffuse;" \
			"uniform vec3 ulightSpecular;" \
			"uniform vec4 uLightPosition;" \
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
			"vec3 lightDirection = normalize(vec3(uLightPosition - eyeCoordinates));" \
			"vec3 reflectionVector = reflect(-lightDirection, transformedNormals);" \
			"vec3 viewerVector = normalize(-eyeCoordinates.xyz);" \
			"vec3 ambientLight = ulightAmbient * umaterialAmbient;" \
			"vec3 diffuseLight = ulightDiffuse * umaterialDiffuse * max(dot(lightDirection, transformedNormals), 0.0f);" \
			"vec3 specularLight = ulightSpecular * umaterialSpecular * (pow(max(dot(reflectionVector, viewerVector), 0.0f), umaterialShininess));" \
			"ophong_ads_light = ambientLight + diffuseLight + specularLight;" \
			"}" \
			"else" \
			"{" \
			"ophong_ads_light = vec3(0.0f , 0.0f , 0.0f);" \
			"}" \
			"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;" \
			"}";



		GLuint vertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShaderObject_PV, 1, (const GLchar**)&vertexShaderSourceCode_PV, NULL);

		// let openGL compile vertex shader
		glCompileShader(vertexShaderObject_PV);

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
		glGetShaderiv(vertexShaderObject_PV, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) // if error
		{
			glGetShaderiv(vertexShaderObject_PV, GL_INFO_LOG_LENGTH, &infoLogLength);

			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);

				if (szInfoLog != NULL)
				{
					glGetShaderInfoLog(vertexShaderObject_PV, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "VertexShader Compilation Error Log : %s\n", szInfoLog);

					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized();
		}


		// FRAGMENT SHADER

		const GLchar* fragmentShaderSourceCode_PV =
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

			GLuint fragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

			glShaderSource(fragmentShaderObject_PV, 1, (const GLchar**)&fragmentShaderSourceCode_PV, NULL);

			glCompileShader(fragmentShaderObject_PV);


			//for enabling shader 
			status = 0;
			infoLogLength = 0;
			szInfoLog = NULL;

			glGetShaderiv(fragmentShaderObject_PV, GL_COMPILE_STATUS, &status);
			if (status == False)
			{
				glGetShaderiv(fragmentShaderObject_PV, GL_INFO_LOG_LENGTH, &infoLogLength);
				if (infoLogLength > 0)
				{
					szInfoLog = (GLchar*)malloc(infoLogLength);
					if (szInfoLog != NULL)
					{
						glGetShaderInfoLog(fragmentShaderObject_PV, infoLogLength, NULL, szInfoLog);
						fprintf(gpFile, "Fragment Shader Compilation Log: \n", szInfoLog);
						free(szInfoLog);
						szInfoLog = NULL;
					}
				}
				uninitialized();
			}

			//shader program
			// create shader program object
			shaderProgramObject_PV = glCreateProgram();

			// attach both shader to this program
			glAttachShader(shaderProgramObject_PV, vertexShaderObject_PV);
			glAttachShader(shaderProgramObject_PV, fragmentShaderObject_PV);

			//bind Attribute location with the shader program object

			glBindAttribLocation(shaderProgramObject_PV, AMC_ATTRIBUTE_POSITION, "aPosition");

			glBindAttribLocation(shaderProgramObject_PV, AMC_ATTRIBUTE_NORMAL, "aNormal");



			//link the shader program
			glLinkProgram(shaderProgramObject_PV);

			//do above shader compilation related steps for linking error 

			status = 0;
			infoLogLength = 0;
			szInfoLog = NULL;

			glGetProgramiv(shaderProgramObject_PV, GL_LINK_STATUS, &status);
			if (status == GL_FALSE)
			{
				glGetProgramiv(shaderProgramObject_PV, GL_INFO_LOG_LENGTH, &infoLogLength);
				if (infoLogLength > 0)
				{
					szInfoLog = (GLchar*)malloc(infoLogLength);
					if (szInfoLog != NULL)
					{
						glGetProgramInfoLog(shaderProgramObject_PV, infoLogLength, NULL, szInfoLog);
						fprintf(gpFile, "Shader Program Linking Error Log: %s\n", szInfoLog);
						free(szInfoLog);
						szInfoLog = NULL;
					}
				}
				uninitialized();
			}

			//get shader uniform locations
			ModelMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uModelMatrix");
			ViewMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uViewMatrix");
			projectionMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uProjectionMatrix");
			lightAmbientUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "ulightAmbient");
			lightDiffuseUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "ulightDiffuse");
			lightSpecularUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "ulightSpecular");
			lightPositionUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uLightPosition");
			materialAmbientUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "umaterialAmbient");
			materialDiffuseUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "umaterialDiffuse");
			materialSpecularUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "umaterialSpecular");
			materialShininessUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "umaterialShininess");
			keypressedUniform = glGetUniformLocation(shaderProgramObject_PV, "ukeypressed");
	}

	// ****************** PER FRAGMENT LIGHT ********************************

	const GLchar* vertexShaderSourceCode_PF =
		"#version 460 core" \
		"\n" \
		"in vec4 aPosition;" \
		"in vec3 aNormal;" \
		"uniform mat4 uModelMatrix;" \
		"uniform mat4 uViewMatrix;" \
		"uniform mat4 uProjectionMatrix;" \
		"uniform vec4 uLightPosition;" \
		"uniform int ukeypressed;" \
		"out vec3 otransformedNormals;" \
		"out vec3 olightDirection;" \
		"out vec3 oviewerVector;" \
		"in vec4 aColor;" \
		"out vec4 oColor;" \
		"void main(void)" \
		"{" \
		"if(ukeypressed == 1)" \
		"{" \
		"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;" \
		"otransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal;" \
		"olightDirection = vec3(uLightPosition - eyeCoordinates);" \
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



	GLuint vertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShaderObject_PF, 1, (const GLchar**)&vertexShaderSourceCode_PF, NULL);

	// let openGL compile vertex shader
	glCompileShader(vertexShaderObject_PF);

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
	glGetShaderiv(vertexShaderObject_PF, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) // if error
	{
		glGetShaderiv(vertexShaderObject_PF, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);

			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject_PF, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "VertexShader Compilation Error Log : %s\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialized();
	}


	// FRAGMENT SHADER

	const GLchar* fragmentShaderSourceCode_PF =
		"#version 460 core" \
		"\n" \
		"in vec4 oColor;" \
		"in vec3 otransformedNormals;" \
		"in vec3 olightDirection;" \
		"in vec3 oviewerVector;" \
		"out vec4 FragColor;" \
		"uniform vec3 ulightAmbient;" \
		"uniform vec3 ulightDiffuse;" \
		"uniform vec3 ulightSpecular;" \
		"uniform vec3 umaterialAmbient;" \
		"uniform vec3 umaterialDiffuse;" \
		"uniform vec3 umaterialSpecular;" \
		"uniform float umaterialShininess;" \
		"uniform int ukeypressed;" \
		"void main(void)" \
		"{" \
		"vec3 Phong_ADS_Light;" \
		"if(ukeypressed == 1)" \
		"{" \
		"vec3 normalizedTransformedNormals = normalize(otransformedNormals);" \
		"vec3 normalizedlightDirection = normalize(olightDirection);" \
		"vec3 normalizedviewerVector = normalize(oviewerVector);" \
		"vec3 ambientLight = ulightAmbient * umaterialAmbient;" \
		"vec3 diffuseLight = ulightDiffuse * umaterialDiffuse * max(dot(normalizedlightDirection, normalizedTransformedNormals), 0.0f);" \
		"vec3 reflectionVector = reflect(-normalizedlightDirection, normalizedTransformedNormals);" \
		"vec3 specularLight = ulightSpecular * umaterialSpecular * (pow(max(dot(reflectionVector, normalizedviewerVector), 0.0f), umaterialShininess));" \
		"Phong_ADS_Light = ambientLight + diffuseLight + specularLight;" \
		"}" \
		"else" \
		"{" \
		"Phong_ADS_Light = vec3(1.0f, 1.0f, 1.0f);" \
		"}" \
		"FragColor = vec4(Phong_ADS_Light, 1.0f);" \
		"}";

	GLuint fragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShaderObject_PF, 1, (const GLchar**)&fragmentShaderSourceCode_PF, NULL);

	glCompileShader(fragmentShaderObject_PF);


	//for enabling shader 
	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(fragmentShaderObject_PF, GL_COMPILE_STATUS, &status);
	if (status == False)
	{
		glGetShaderiv(fragmentShaderObject_PF, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject_PF, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log: \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialized();
	}

	//shader program
	// create shader program object
	shaderProgramObject_PF = glCreateProgram();

	// attach both shader to this program
	glAttachShader(shaderProgramObject_PF, vertexShaderObject_PF);
	glAttachShader(shaderProgramObject_PF, fragmentShaderObject_PF);

	//bind Attribute location with the shader program object

	glBindAttribLocation(shaderProgramObject_PF, AMC_ATTRIBUTE_POSITION, "aPosition");


	glBindAttribLocation(shaderProgramObject_PF, AMC_ATTRIBUTE_NORMAL, "aNormal");



	//link the shader program
	glLinkProgram(shaderProgramObject_PF);

	//do above shader compilation related steps for linking error 

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject_PF, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_PF, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_PF, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program Linking Error Log: %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialized();
	}

	//get shader uniform locations
	ModelMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uModelMatrix");
	ViewMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uViewMatrix");
	projectionMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uProjectionMatrix");
	lightAmbientUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "ulightAmbient");
	lightDiffuseUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "ulightDiffuse");
	lightSpecularUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "ulightSpecular");
	lightPositionUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uLightPosition");
	materialAmbientUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "umaterialAmbient");
	materialDiffuseUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "umaterialDiffuse");
	materialSpecularUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "umaterialSpecular");
	materialShininessUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "umaterialShininess");
	keypressedUniform = glGetUniformLocation(shaderProgramObject_PF, "ukeypressed");



	//// sphere vertices arrays

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);

	gNumVertices = getNumberOfSphereVertices();

	gNumElements = getNumberOfSphereElements();

	//create vertex array object
	glGenVertexArrays(1, &gVao_sphere);

	//bind with vao
	glBindVertexArray(gVao_sphere);

	// VBO for position
	//create vertex buffer array for position
	glGenBuffers(1, &gVbo_sphere_position);

	//bind with vbo of position
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

		//unbind with vbo of positin
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &gVbo_sphere_normal);
		glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

		glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		
	
		glGenBuffers(1, &vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_textures), sphere_textures, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORDS, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORDS);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// element vbo
		glGenBuffers(1, &gVbo_sphere_element);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// unbind vao
		glBindVertexArray(0);

	// enabling depth tests
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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

	 //per vertex and per fragment with toggling

	if(bLightEnabled_PV == True) // v
	{
		glUseProgram(shaderProgramObject_PV);
	}
	else if(bLightEnabled_PF == True)
	{
		glUseProgram(shaderProgramObject_PF);
	}
	else
	{
		glUseProgram(shaderProgramObject_PV);
	}
	
	//sphere

	mat4 modelMatrix = mat4::identity();
	mat4 ViewMatrix = mat4::identity();

	mat4 translationMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);

	mat4 scaleMatrix = mat4::identity();
	scaleMatrix = vmath::scale(2.0f, 2.0f, 2.0f);

	modelMatrix = translationMatrix * scaleMatrix;



	//light 
	if (bLightingEnabled == True)
	{

		if (bLightEnabled_PV == True)
		{
			glUniformMatrix4fv(ModelMatrixUniform_PV, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(ViewMatrixUniform_PV, 1, GL_FALSE, ViewMatrix);
			glUniformMatrix4fv(projectionMatrixUniform_PV, 1, GL_FALSE, perspectiveProjectionMatrix);

			glUniform1i(keypressedUniform, 1);
			glUniform3fv(lightAmbientUniform_PV, 1, lightAmbient);
			glUniform3fv(lightDiffuseUniform_PV, 1, lightDiffuse);
			glUniform3fv(lightSpecularUniform_PV, 1, lightSpecular);
			glUniform3fv(materialAmbientUniform_PV, 1, materialAmbient);
			glUniform3fv(materialDiffuseUniform_PV, 1, materialDiffuse);
			glUniform3fv(materialSpecularUniform_PV, 1, materialSpecular);
			glUniform4fv(lightPositionUniform_PV, 1, lightPosition);
			glUniform1f(materialShininessUniform_PV, materialShininess);
		}

		if (bLightEnabled_PF == True)
		{
			glUniformMatrix4fv(ModelMatrixUniform_PF, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(ViewMatrixUniform_PF, 1, GL_FALSE, ViewMatrix);
			glUniformMatrix4fv(projectionMatrixUniform_PF, 1, GL_FALSE, perspectiveProjectionMatrix);

			glUniform1i(keypressedUniform, 1);
			glUniform3fv(lightAmbientUniform_PF, 1, lightAmbient);
			glUniform3fv(lightDiffuseUniform_PF, 1, lightDiffuse);
			glUniform3fv(lightSpecularUniform_PF, 1, lightSpecular);
			glUniform3fv(materialAmbientUniform_PF, 1, materialAmbient);
			glUniform3fv(materialDiffuseUniform_PF, 1, materialDiffuse);
			glUniform3fv(materialSpecularUniform_PF, 1, materialSpecular);
			glUniform4fv(lightPositionUniform_PF, 1, lightPosition);
			glUniform1f(materialShininessUniform_PF, materialShininess);
		}
	}
	else
	{
		glUniformMatrix4fv(ModelMatrixUniform_PV, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewMatrixUniform_PV, 1, GL_FALSE, ViewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform_PV, 1, GL_FALSE, perspectiveProjectionMatrix);
		glUniform1i(keypressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);


	//draw geometry / model / scenes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

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
	if (shaderProgramObject_PV)
	{
		glUseProgram(shaderProgramObject_PV);

		GLint numShaders = 0;

		glGetProgramiv(shaderProgramObject_PV, GL_ATTACHED_SHADERS, &numShaders);

		// create and allocate array of shader object according to the number of attached shaders
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));

			//get shader objects into this allocated array

			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_PV, numShaders, NULL, pShaders);

			}
			for (GLint i = 0; i < numShaders; i++)
			{
				glDetachShader(shaderProgramObject_PV, pShaders[i]);
				glDeleteShader(pShaders[i]);
				pShaders[i] = 0;
			}

			free(pShaders);
			pShaders = NULL;
		}
		//unuse
		glUseProgram(0);
		// del shader prog object
		glDeleteProgram(shaderProgramObject_PV);

		shaderProgramObject_PV = 0;

	}


	if (shaderProgramObject_PF)
	{
		glUseProgram(shaderProgramObject_PF);

		GLint numShaders = 0;

		glGetProgramiv(shaderProgramObject_PF, GL_ATTACHED_SHADERS, &numShaders);

		// create and allocate array of shader object according to the number of attached shaders
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));

			//get shader objects into this allocated array

			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_PF, numShaders, NULL, pShaders);

			}
			for (GLint i = 0; i < numShaders; i++)
			{
				glDetachShader(shaderProgramObject_PF, pShaders[i]);
				glDeleteShader(pShaders[i]);
				pShaders[i] = 0;
			}

			free(pShaders);
			pShaders = NULL;
		}
		//unuse
		glUseProgram(0);
		// del shader prog object
		glDeleteProgram(shaderProgramObject_PF);

		shaderProgramObject_PF = 0;

	}

	void ToggleFullscreen(void);
	if (gbFullscreen == True)
	{
		ToggleFullscreen();
		gbFullscreen = False;
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



