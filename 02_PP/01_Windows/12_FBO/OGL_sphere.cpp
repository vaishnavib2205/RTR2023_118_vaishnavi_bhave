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
BOOL gbActive = FALSE;

DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
BOOL gbFullscreen = FALSE;

//OpenGL related Global variables
HDC ghdc = NULL;   // DC
HGLRC ghrc = NULL; // rendating context

GLuint shaderProgramObject_PV_sphere = 0;

GLuint shaderProgramObject_PF_sphere = 0;

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
GLuint vbo_texcoords_sphere = 0;

GLuint gNumVertices;
GLuint gNumElements;

GLuint ModelMatrixUniform_PV_sphere = 0;
GLuint ViewMatrixUniform_PV_sphere = 0;
GLuint projectionMatrixUniform_PV_sphere = 0;

GLuint lightDiffuseUniform_PV_sphere[3];
GLuint lightAmbientUniform_PV_sphere[3];
GLuint lightSpecularUniform_PV_sphere[3];
GLuint lightPositionUniform_PV_sphere[3];

GLuint materialDiffuseUniform_PV_sphere = 0;
GLuint materialAmbientUniform_PV_sphere = 0;
GLuint materialSpecularUniform_PV_sphere = 0;
GLuint materialShininessUniform_PV_sphere = 0;

GLuint keypressedUniform_sphere = 0;

GLuint ModelMatrixUniform_PF_sphere = 0;
GLuint ViewMatrixUniform_PF_sphere = 0;
GLuint projectionMatrixUniform_PF_sphere = 0;

GLuint lightDiffuseUniform_PF_sphere[3];
GLuint lightAmbientUniform_PF_sphere[3];
GLuint lightSpecularUniform_PF_sphere[3];
GLuint lightPositionUniform_PF_sphere[3];

GLuint materialDiffuseUniform_PF_sphere = 0;
GLuint materialAmbientUniform_PF_sphere = 0;
GLuint materialSpecularUniform_PF_sphere = 0;
GLuint materialShininessUniform_PF_sphere = 0;


BOOL bLightingEnabled_sphere = FALSE;
BOOL bAnimationEnabled_sphere = FALSE;


BOOL bLightEnabled_PV_sphere = FALSE;
BOOL bLightEnabled_PF_sphere = FALSE;

struct Light_sphere
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec4 position;
};

struct Light_sphere light[3];

GLfloat materialAmbient[] = { 0.0f , 0.0f , 0.0f , 1.0f };
GLfloat materialDiffuse[] = { 0.5f , 0.2f , 0.7f , 1.0f };
GLfloat materialSpecular[] = { 0.7f , 0.7f , 0.7f , 1.0f };
GLfloat materialShininess = 50.0f;


GLfloat lightAngleZero_sphere = 0.0f;
GLfloat lightAngleOne_sphere = 0.0f;
GLfloat lightAngleTwo_sphere = 0.0f;

mat4 perspectiveProjectionMatrix_sphere = 0;
//mat4 -> 4 by 4 cha matrix -> is in vmath.h


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCnmdLine, int iCmdShow) // function defition
{

	// Function Declarations

	int initialize_sphere(void);
	void uninitialized_sphere(void);
	void display_sphere(void);
	void update_sphere(void);

	// Local variable Declarations

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("VBWindow");
	int iResult = 0;
	BOOL bDone = FALSE;

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

	iResult = initialize_sphere();

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
				display_sphere();

					// Update
				update_sphere();
				

			}
		}
	}

	//uninitialization
	uninitialized_sphere();

	return((int)msg.wParam);
}

// callBack Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration

	void resize_sphere(int, int);

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
		resize_sphere(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_ERASEBKGND:
		return 0;


	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
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

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'Q':
		case 'q':
			DestroyWindow(hwnd);
			break;

		case 'L':
		case 'l':
			if (bLightingEnabled_sphere == FALSE)
			{
				bLightingEnabled_sphere = TRUE;
			}
			else
			{
				bLightingEnabled_sphere = FALSE;
			}
			break;

		case 'V':
		case 'v':
			if (bLightEnabled_PV_sphere == FALSE)
			{
				bLightEnabled_PV_sphere = TRUE;
			}
			else
			{
				bLightEnabled_PV_sphere = FALSE;
			}
			break;

		case 'F':
		case 'f':
			if (bLightEnabled_PF_sphere == FALSE)
			{

				bLightEnabled_PF_sphere = TRUE;
			}
			else
			{
				bLightEnabled_PF_sphere = FALSE;
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

int initialize_sphere(void)
{
	// Function Declarations

	void resize_sphere(int, int);
	void printGLInfo(void);
	void uninitialized_sphere(void);

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

	//mathematical steps for diffuse light
	//diffuse light = ld * kd * (s n);
	//step 1: calculate i coords by multiplying modelviewMatrix and position attribute vectors
	//step 2: calculate normal matrix by taking 3 * 3 matrix from upper view of model view matrix by inverting and transposing 
	//step 3: Transform normals by normalising the multiplication of normal matrix and normal attributes
	//step 4: calculate light source vector by normalising the subtraction of lightposition vector and iPosition vector 
	// step 5: finally the diffuse light will be ld * kd * (s.n);

	// ****************** PER VERTEX LIGHT ********************************
	{

		// vertex shader
		const GLchar* vertexShaderSourceCode_PV =
			"#version 460 core" \
			"\n" \
			"in vec4 aPosition;" \
			"in vec3 aNormal;" \
			"uniform mat4 uModelMatrix;" \
			"uniform mat4 uViewMatrix;" \
			"uniform mat4 uProjectionMatrix;" \
			"uniform vec3 ulightAmbient[3];" \
			"uniform vec3 ulightDiffuse[3];" \
			"uniform vec3 ulightSpecular[3];" \
			"uniform vec4 uLightPosition[3];" \
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
			"ophong_ads_light = vec3(0.0f , 0.0f , 0.0f);" \
			"if(ukeypressed == 1)" \
			"{" \
			"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;" \
			"vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);" \
			"vec3 lightDirection[3];" \
			"vec3 reflectionVector[3];" \
			"vec3 viewerVector = normalize(-eyeCoordinates.xyz);" \
			"vec3 ambientLight[3];" \
			"vec3 diffuseLight[3];" \
			"vec3 specularLight[3];" \
			"for(int i = 0; i<3; i++)" \
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

		GLuint vertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShaderObject_PV, 1, (const GLchar**)&vertexShaderSourceCode_PV, NULL);

		glCompileShader(vertexShaderObject_PV);

		// check for vertex shader compilation error if any

		GLint status = 0;
		GLint infoLogLength = 0;
		GLchar* szInfoLog = NULL;

		glGetShaderiv(vertexShaderObject_PV, GL_COMPILE_STATUS, &status);
		if (status == FALSE)
		{
			glGetShaderiv(vertexShaderObject_PV, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetShaderInfoLog(vertexShaderObject_PV, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Vertex Shader Compilation Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_sphere();
		}

		// fragment shader

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

		// for fragment shader

		status = 0;
		infoLogLength = 0;
		szInfoLog = NULL;

		glGetShaderiv(fragmentShaderObject_PV, GL_COMPILE_STATUS, &status);
		if (status == FALSE)
		{
			glGetShaderiv(fragmentShaderObject_PV, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetShaderInfoLog(fragmentShaderObject_PV, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Fragment Shader Compilation Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_sphere();
		}

		// create shader program

		shaderProgramObject_PV_sphere = glCreateProgram();

		glAttachShader(shaderProgramObject_PV_sphere, vertexShaderObject_PV);
		glAttachShader(shaderProgramObject_PV_sphere, fragmentShaderObject_PV);

		glBindAttribLocation(shaderProgramObject_PV_sphere, AMC_ATTRIBUTE_POSITION, "aPosition");
		glBindAttribLocation(shaderProgramObject_PV_sphere, AMC_ATTRIBUTE_NORMAL, "aNormal");

		glLinkProgram(shaderProgramObject_PV_sphere);

		// for shader compilation

		status = 0;
		infoLogLength = 0;
		szInfoLog = NULL;

		glGetProgramiv(shaderProgramObject_PV_sphere, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			glGetProgramiv(shaderProgramObject_PV_sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetProgramInfoLog(shaderProgramObject_PV_sphere, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Shader Program linking Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_sphere();
		}

		// get shader uniform location
		ModelMatrixUniform_PV_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uModelMatrix");
		ViewMatrixUniform_PV_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uViewMatrix");
		projectionMatrixUniform_PV_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "uProjectionMatrix");

		lightAmbientUniform_PV_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulightAmbient[0]");
		lightDiffuseUniform_PV_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulightDiffuse[0]");
		lightSpecularUniform_PV_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulightSpecular[0]");
		lightPositionUniform_PV_sphere[0] = glGetUniformLocation(shaderProgramObject_PV_sphere, "uLightPosition[0]");

		lightAmbientUniform_PV_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulightAmbient[1]");
		lightDiffuseUniform_PV_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulightDiffuse[1]");
		lightSpecularUniform_PV_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulightSpecular[1]");
		lightPositionUniform_PV_sphere[1] = glGetUniformLocation(shaderProgramObject_PV_sphere, "uLightPosition[1]");


		lightAmbientUniform_PV_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulightAmbient[2]");
		lightDiffuseUniform_PV_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulightDiffuse[2]");
		lightSpecularUniform_PV_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "ulightSpecular[2]");
		lightPositionUniform_PV_sphere[2] = glGetUniformLocation(shaderProgramObject_PV_sphere, "uLightPosition[2]");


		materialAmbientUniform_PV_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "umaterialAmbient");
		materialDiffuseUniform_PV_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "umaterialDiffuse");
		materialSpecularUniform_PV_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "umaterialSpecular");
		materialShininessUniform_PV_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "umaterialShininess");


		keypressedUniform_sphere = glGetUniformLocation(shaderProgramObject_PV_sphere, "ukeypressed");
	}

	// ****************** PER FRAGMENT LIGHT ********************************


	{
		const GLchar* vertexShaderSourceCode_PF_sphere =
			"#version 460 core" \
			"\n" \
			"in vec4 aPosition;" \
			"in vec3 aNormal;" \
			"uniform mat4 uModelMatrix;" \
			"uniform mat4 uViewMatrix;" \
			"uniform mat4 uProjectionMatrix;" \
			"out vec3 otransformedNormals;" \
			"out vec3 oviewerVector;" \
			"out vec4 oeyeCoordinates;" \
			"uniform int ukeypressed;" \
			"in vec4 aColor;" \
			"out vec4 oColor;" \
			"void main(void)" \
			"{" \
			"if(ukeypressed == 1)" \
			"{" \
			"oeyeCoordinates = uViewMatrix * uModelMatrix * aPosition;" \
			"otransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal;" \
			"oviewerVector = -oeyeCoordinates.xyz;" \
			"}" \
			"else" \
			"{" \
			"otransformedNormals = vec3(0.0f , 0.0f , 0.0f);" \
			"oviewerVector = vec3(0.0f , 0.0f , 0.0f);" \
			"}" \
			"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;" \
			"}";

		// create vertex shader object

		GLuint vertexShaderObject_PF_sphere = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShaderObject_PF_sphere, 1, (const GLchar**)&vertexShaderSourceCode_PF_sphere, NULL);

		glCompileShader(vertexShaderObject_PF_sphere);

		// check for vertex shader compilation error if any

		GLint status = 0;
		GLint infoLogLength = 0;
		GLchar* szInfoLog = NULL;

		glGetShaderiv(vertexShaderObject_PF_sphere, GL_COMPILE_STATUS, &status);
		if (status == FALSE)
		{
			glGetShaderiv(vertexShaderObject_PF_sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetShaderInfoLog(vertexShaderObject_PF_sphere, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Vertex Shader Compilation Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_sphere();
		}

		// fragment shader

		const GLchar* fragmentShaderSourceCode_PF_sphere =
			"#version 460 core" \
			"\n" \
			"in vec4 oColor;" \
			"in vec3 otransformedNormals;" \
			"in vec4 oeyeCoordinates;" \
			"in vec3 oviewerVector;" \
			"uniform vec3 ulightAmbient[3];" \
			"uniform vec3 ulightDiffuse[3];" \
			"uniform vec3 ulightSpecular[3];" \
			"uniform vec4 uLightPosition[3];" \
			"uniform vec3 umaterialAmbient;" \
			"uniform vec3 umaterialDiffuse;" \
			"uniform vec3 umaterialSpecular;" \
			"uniform float umaterialShininess;" \
			"out vec4 FragColor;" \
			"uniform int ukeypressed;" \
			"void main(void)" \
			"{" \
			"vec3 Phong_ADS_Light;" \
			"if(ukeypressed == 1)" \
			"{" \
			"vec3 lightDirection[3];" \
			"vec3 reflectionVector[3];" \
			"vec3 ambientLight[3];" \
			"vec3 diffuseLight[3];" \
			"vec3 specularLight[3];" \
			"vec3 normalizedTransformedNormals = normalize(otransformedNormals);" \
			"for(int i = 0; i<3; i++)" \
			"{" \
			"ambientLight[i] = ulightAmbient[i] * umaterialAmbient;" \
			"lightDirection[i] = normalize(vec3(uLightPosition[i] - oeyeCoordinates));" \
			"diffuseLight[i] =ulightDiffuse[i] * umaterialDiffuse * max(dot(lightDirection[i], normalizedTransformedNormals), 0.0f);" \
			"reflectionVector[i] = reflect(-lightDirection[i], normalizedTransformedNormals);" \
			"vec3 normalizedviewerVector = normalize(oviewerVector);" \
			"specularLight[i] = ulightSpecular[i] * umaterialSpecular * pow(max(dot(reflectionVector[i], normalizedviewerVector), 0.0f), umaterialShininess);" \
			"Phong_ADS_Light = Phong_ADS_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];" \
			"}" \
			"FragColor = vec4(Phong_ADS_Light, 1.0f);" \
			"}" \
			"else" \
			"{" \
			"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0);" \
			"}" \
			"}";

		GLuint fragmentShaderObject_PF_sphere = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(fragmentShaderObject_PF_sphere, 1, (const GLchar**)&fragmentShaderSourceCode_PF_sphere, NULL);

		glCompileShader(fragmentShaderObject_PF_sphere);

		// for fragment shader

		status = 0;
		infoLogLength = 0;
		szInfoLog = NULL;

		glGetShaderiv(fragmentShaderObject_PF_sphere, GL_COMPILE_STATUS, &status);
		if (status == FALSE)
		{
			glGetShaderiv(fragmentShaderObject_PF_sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetShaderInfoLog(fragmentShaderObject_PF_sphere, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Fragment Shader Compilation Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_sphere();
		}

		// create shader program

		shaderProgramObject_PF_sphere = glCreateProgram();

		glAttachShader(shaderProgramObject_PF_sphere, vertexShaderObject_PF_sphere);
		glAttachShader(shaderProgramObject_PF_sphere, fragmentShaderObject_PF_sphere);

		glBindAttribLocation(shaderProgramObject_PF_sphere, AMC_ATTRIBUTE_POSITION, "aPosition");
		glBindAttribLocation(shaderProgramObject_PF_sphere, AMC_ATTRIBUTE_NORMAL, "aNormal");

		glLinkProgram(shaderProgramObject_PF_sphere);

		// for shader compilation

		status = 0;
		infoLogLength = 0;
		szInfoLog = NULL;

		glGetProgramiv(shaderProgramObject_PF_sphere, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			glGetProgramiv(shaderProgramObject_PF_sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetProgramInfoLog(shaderProgramObject_PF_sphere, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Shader Program linking Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_sphere();
		}

		// get shader uniform location
		ModelMatrixUniform_PF_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "uModelMatrix");
		ViewMatrixUniform_PF_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "uViewMatrix");
		projectionMatrixUniform_PF_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "uProjectionMatrix");

		lightAmbientUniform_PF_sphere[0] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulightAmbient[0]");
		lightDiffuseUniform_PF_sphere[0] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulightDiffuse[0]");
		lightSpecularUniform_PF_sphere[0] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulightSpecular[0]");
		lightPositionUniform_PF_sphere[0] = glGetUniformLocation(shaderProgramObject_PF_sphere, "uLightPosition[0]");

		lightAmbientUniform_PF_sphere[1] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulightAmbient[1]");
		lightDiffuseUniform_PF_sphere[1] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulightDiffuse[1]");
		lightSpecularUniform_PF_sphere[1] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulightSpecular[1]");
		lightPositionUniform_PF_sphere[1] = glGetUniformLocation(shaderProgramObject_PF_sphere, "uLightPosition[1]");

		lightAmbientUniform_PF_sphere[2] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulightAmbient[2]");
		lightDiffuseUniform_PF_sphere[2] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulightDiffuse[2]");
		lightSpecularUniform_PF_sphere[2] = glGetUniformLocation(shaderProgramObject_PF_sphere, "ulightSpecular[2]");
		lightPositionUniform_PF_sphere[2] = glGetUniformLocation(shaderProgramObject_PF_sphere, "uLightPosition[2]");


		materialAmbientUniform_PF_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "umaterialAmbient");
		materialDiffuseUniform_PF_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "umaterialDiffuse");
		materialSpecularUniform_PF_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "umaterialSpecular");
		materialShininessUniform_PF_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "umaterialShininess");


		keypressedUniform_sphere = glGetUniformLocation(shaderProgramObject_PF_sphere, "ukeypressed");
	}


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



	glGenBuffers(1, &vbo_texcoords_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_sphere);
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



	light[0].ambient = vec3{ 0.0f, 0.0f, 0.0f };
	light[1].ambient = vec3{ 0.0f, 0.0f, 0.0f };
	light[2].ambient = vec3{ 0.0f, 0.0f, 0.0f };

	light[0].diffuse = vec3{ 1.0f, 0.0f, 0.0f };
	light[1].diffuse = vec3{ 0.0f, 1.0f, 0.0f };
	light[2].diffuse = vec3{ 0.0f, 0.0f, 1.0f };

	light[0].specular = vec3{ 1.0f, 0.0f, 0.0f };
	light[1].specular = vec3{ 0.0f, 1.0f, 0.0f };
	light[2].specular = vec3{ 0.0f, 0.0f, 1.0f };


	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//initialize orthographic projection matrix
	perspectiveProjectionMatrix_sphere = vmath::mat4::identity();


	// Here OpenGL starts.....
	resize_sphere(WIN_WIDTH, WIN_HEIGHT);

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

void resize_sphere(int width, int height)
{
	// code
	if (height <= 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//set orthographic projection matrix
	perspectiveProjectionMatrix_sphere = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);

}

void display_sphere(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ***********************STEPS FOR DISPLAY**************************

	//1. use shader prog object 
	// 2. bind with vao

	 //per vertex and per fragment with toggling

	if (bLightEnabled_PV_sphere == TRUE) // v
	{
		glUseProgram(shaderProgramObject_PV_sphere);
	}
	else if (bLightEnabled_PF_sphere == TRUE)
	{
		glUseProgram(shaderProgramObject_PF_sphere);
	}
	else
	{
		glUseProgram(shaderProgramObject_PV_sphere);
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
	if (bLightingEnabled_sphere == TRUE)
	{

		if (bLightEnabled_PV_sphere == TRUE)
		{
			glUniformMatrix4fv(ModelMatrixUniform_PV_sphere, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(ViewMatrixUniform_PV_sphere, 1, GL_FALSE, ViewMatrix);
			glUniformMatrix4fv(projectionMatrixUniform_PV_sphere, 1, GL_FALSE, perspectiveProjectionMatrix_sphere);

			glUniform1i(keypressedUniform_sphere, 1);
			glUniform3fv(lightAmbientUniform_PV_sphere[0], 1, light[0].ambient);
			glUniform3fv(lightDiffuseUniform_PV_sphere[0], 1, light[0].diffuse);
			glUniform3fv(lightSpecularUniform_PV_sphere[0], 1, light[0].specular);

			glUniform3fv(lightAmbientUniform_PV_sphere[1], 1, light[1].ambient);
			glUniform3fv(lightDiffuseUniform_PV_sphere[1], 1, light[1].diffuse);
			glUniform3fv(lightSpecularUniform_PV_sphere[1], 1, light[1].specular);

			glUniform3fv(lightAmbientUniform_PV_sphere[2], 1, light[2].ambient);
			glUniform3fv(lightDiffuseUniform_PV_sphere[2], 1, light[2].diffuse);
			glUniform3fv(lightSpecularUniform_PV_sphere[2], 1, light[2].specular);


			glUniform3fv(materialAmbientUniform_PV_sphere, 1, materialAmbient);
			glUniform3fv(materialDiffuseUniform_PV_sphere, 1, materialDiffuse);
			glUniform3fv(materialSpecularUniform_PV_sphere, 1, materialSpecular);
			glUniform1f(materialShininessUniform_PV_sphere, materialShininess);


			glUniform4fv(lightPositionUniform_PV_sphere[0], 1, light[0].position);
			glUniform4fv(lightPositionUniform_PV_sphere[1], 1, light[1].position);
			glUniform4fv(lightPositionUniform_PV_sphere[2], 1, light[2].position);
		}

		if (bLightEnabled_PF_sphere == TRUE)
		{
			glUniformMatrix4fv(ModelMatrixUniform_PF_sphere, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(ViewMatrixUniform_PF_sphere, 1, GL_FALSE, ViewMatrix);
			glUniformMatrix4fv(projectionMatrixUniform_PF_sphere, 1, GL_FALSE, perspectiveProjectionMatrix_sphere);

			glUniform1i(keypressedUniform_sphere, 1);
			glUniform3fv(lightAmbientUniform_PF_sphere[0], 1, light[0].ambient);
			glUniform3fv(lightDiffuseUniform_PF_sphere[0], 1, light[0].diffuse);
			glUniform3fv(lightSpecularUniform_PF_sphere[0], 1, light[0].specular);

			glUniform3fv(lightAmbientUniform_PF_sphere[1], 1, light[1].ambient);
			glUniform3fv(lightDiffuseUniform_PF_sphere[1], 1, light[1].diffuse);
			glUniform3fv(lightSpecularUniform_PF_sphere[1], 1, light[1].specular);

			glUniform3fv(lightAmbientUniform_PF_sphere[2], 1, light[2].ambient);
			glUniform3fv(lightDiffuseUniform_PF_sphere[2], 1, light[2].diffuse);
			glUniform3fv(lightSpecularUniform_PF_sphere[2], 1, light[2].specular);


			glUniform3fv(materialAmbientUniform_PF_sphere, 1, materialAmbient);
			glUniform3fv(materialDiffuseUniform_PF_sphere, 1, materialDiffuse);
			glUniform3fv(materialSpecularUniform_PF_sphere, 1, materialSpecular);
			glUniform1f(materialShininessUniform_PF_sphere, materialShininess);


			glUniform4fv(lightPositionUniform_PF_sphere[0], 1, light[0].position);
			glUniform4fv(lightPositionUniform_PF_sphere[1], 1, light[1].position);
			glUniform4fv(lightPositionUniform_PF_sphere[2], 1, light[2].position);
		}
	}
	else
	{
		glUniformMatrix4fv(ModelMatrixUniform_PV_sphere, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewMatrixUniform_PV_sphere, 1, GL_FALSE, ViewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform_PV_sphere, 1, GL_FALSE, perspectiveProjectionMatrix_sphere);
		glUniform1i(keypressedUniform_sphere, 0);
	}

	glBindVertexArray(gVao_sphere);


	//draw geometry / model / scenes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	glUseProgram(0);


	SwapBuffers(ghdc);

}
 
void update_sphere(void)
{
	// Code
	if (bLightingEnabled_sphere == TRUE)
	{
		light[0].position[0] = 0.0f;
		light[0].position[1] = 5.0f * (GLfloat)sin(lightAngleZero_sphere);
		light[0].position[2] = 5.0f * (GLfloat)cos(lightAngleZero_sphere);
		light[0].position[3] = 1.0f;


		light[1].position[0] = 5.0f * (GLfloat)cos(lightAngleOne_sphere);
		light[1].position[1] = 0.0f;
		light[1].position[2] = 5.0f * (GLfloat)sin(lightAngleOne_sphere);
		light[1].position[3] = 1.0f;


		light[2].position[0] = 5.0f * (GLfloat)cos(lightAngleTwo_sphere);
		light[2].position[1] = 5.0f * (GLfloat)sin(lightAngleTwo_sphere);
		light[2].position[2] = 0.0f;
		light[2].position[3] = 1.0f;

	lightAngleZero_sphere = lightAngleZero_sphere + 0.0005f;
	if (lightAngleZero_sphere >= 360.0f)
	{
		lightAngleZero_sphere -= 360.0f;
	}
	
	lightAngleOne_sphere = lightAngleOne_sphere + 0.0005f;
	if (lightAngleOne_sphere >= 360.0f)
	{
		lightAngleOne_sphere -= 360.0f;
	}
	
	lightAngleTwo_sphere = lightAngleTwo_sphere + 0.0005f;
	if (lightAngleTwo_sphere >= 360.0f)
	{
		lightAngleTwo_sphere -= 360.0f;
	}
	}


}

void uninitialized_sphere(void)
{
	fprintf(gpFile, "__________________________\n");
	// *****************STEPS FOR SHADER PROG AND OBJ UNINITIALISATION*************************
	// 1. use shader Program object
	// 2. get number of attached shaders 
	// 3. Detach and delete shader objects iteratively or in loop
	if (shaderProgramObject_PV_sphere)
	{
		glUseProgram(shaderProgramObject_PV_sphere);

		GLint numShaders = 0;

		glGetProgramiv(shaderProgramObject_PV_sphere, GL_ATTACHED_SHADERS, &numShaders);

		// create and allocate array of shader object according to the number of attached shaders
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));

			//get shader objects into this allocated array

			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_PV_sphere, numShaders, NULL, pShaders);

			}
			for (GLint i = 0; i < numShaders; i++)
			{
				glDetachShader(shaderProgramObject_PV_sphere, pShaders[i]);
				glDeleteShader(pShaders[i]);
				pShaders[i] = 0;
			}

			free(pShaders);
			pShaders = NULL;
		}
		//unuse
		glUseProgram(0);
		// del shader prog object
		glDeleteProgram(shaderProgramObject_PV_sphere);

		shaderProgramObject_PV_sphere = 0;

	}


	if (shaderProgramObject_PF_sphere)
	{
		glUseProgram(shaderProgramObject_PF_sphere);

		GLint numShaders = 0;

		glGetProgramiv(shaderProgramObject_PF_sphere, GL_ATTACHED_SHADERS, &numShaders);

		// create and allocate array of shader object according to the number of attached shaders
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));

			//get shader objects into this allocated array

			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_PF_sphere, numShaders, NULL, pShaders);

			}
			for (GLint i = 0; i < numShaders; i++)
			{
				glDetachShader(shaderProgramObject_PF_sphere, pShaders[i]);
				glDeleteShader(pShaders[i]);
				pShaders[i] = 0;
			}

			free(pShaders);
			pShaders = NULL;
		}
		//unuse
		glUseProgram(0);
		// del shader prog object
		glDeleteProgram(shaderProgramObject_PF_sphere);

		shaderProgramObject_PF_sphere = 0;

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



