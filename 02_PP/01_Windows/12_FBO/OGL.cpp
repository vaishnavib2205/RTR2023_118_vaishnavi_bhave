#include<Windows.h> // Windows header files 
#include "OGL.h"
#include<stdlib.h> // For exit
#include<stdio.h> // For file IO (input output)

// OpenGL header files
#include<gl/glew.h> // this must be before gl/GL.h
#include<gl/GL.h>

// header file for vmath
#include "vmath.h"
#include "Sphere.h"


using namespace vmath;

// MACROS

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//macros for fbo

#define FBO_WIDTH 512
#define FBO_HEIGHT 512

// Link with OpenGL libraray

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"Sphere.lib")


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


enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_TEXCOORD,
	AMC_ATTRIBUTE_NORMALS

};

//*******************************************************************

// variables for cube

GLuint shaderProgramObject_cube = 0;
GLuint vao_Cube = 0;
GLuint vbo_position_cube = 0;
GLuint vbo_texcoord_cube = 0;
GLfloat angle_cube = 0.0f;


GLint WinWidth;
GLint WinHeight;

GLuint mvpMatrixUniform_cube = 0;
GLuint textureSamplerUniform_cube = 0;

mat4 perspectiveProjectionMatrix_cube; // mat4 datatype is in vmath.h header file, that means 4X4 matrix

//***********************************************************************************************************************************

//variables/arrays related to sphere
GLuint shaderProgramObject_PV_sphere = 0;

GLuint shaderProgramObject_PF_sphere = 0;

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

//************************************************************************************

//FBO related global variables

GLuint fbo;
GLuint rbo;
GLuint textureFBO;
BOOL fboResult = FALSE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCnmdLine, int iCmdShow) // function defition
{

	// Function Declarations
	
	int initialize_cube(void);
	void uninitialized_cube(void);
	void display_cube(void);
	void update_cube(void);

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

	iResult = initialize_cube();

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
				display_cube();

				// Update
				update_cube();

			}
		}
	}

	//uninitialization
	uninitialized_cube();

	return((int)msg.wParam);
}

// callBack Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration

	void resize_cube(int, int);

	void ToggleFullscreen(void);

	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Hello World !!!");

	// code
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
		resize_cube(LOWORD(lParam), HIWORD(lParam));
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

int initialize_cube(void)
{
	// Function Declarations

	void resize_cube(int, int);
	void printGLInfo(void);
	int initialize_sphere(GLint, GLint);
	void uninitialized_cube(void);
	BOOL createFBO(GLint, GLint);

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
		"in vec2 aTexCoord;" \
		"out vec2 oTexCoord;" \
		"uniform mat4 uMVPMatrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = uMVPMatrix * aPosition;" \
		"oTexCoord = aTexCoord;" \
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
		uninitialized_cube();
	}

	// fragment shader

	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec2 oTexCoord;" \
		"uniform sampler2D uTextureSampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = texture(uTextureSampler, oTexCoord);" \
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
		uninitialized_cube();
	}

	// create shader program

	shaderProgramObject_cube = glCreateProgram();

	glAttachShader(shaderProgramObject_cube, vertexShaderObject);
	glAttachShader(shaderProgramObject_cube, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject_cube, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject_cube, AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");

	glLinkProgram(shaderProgramObject_cube);
	
	// for shader compilation

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject_cube, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_cube, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_cube, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program linking Error Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialized_cube();
	}

	// get shader uniform location

	mvpMatrixUniform_cube = glGetUniformLocation(shaderProgramObject_cube, "uMVPMatrix");
	textureSamplerUniform_cube = glGetUniformLocation(shaderProgramObject_cube, "uTextureSampler");

	// declare position and color arrays

	const GLfloat cube_position[] =
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

	const GLfloat cube_texcoord[] =
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

		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_position), cube_position, GL_STATIC_DRAW);

		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// for white color

		glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

		glGenBuffers(1, &vbo_texcoord_cube);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord_cube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoord), cube_texcoord, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoord), cube_texcoord, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// unbind with vao
		glBindVertexArray(0);
	}


	glEnable(GL_TEXTURE_2D);
	// enabling depth 

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	// set the clear colour of window to blue

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Here OpenGL starts.....

	perspectiveProjectionMatrix_cube = vmath::mat4::identity();


	resize_cube(WIN_WIDTH, WIN_HEIGHT);

	if (createFBO(FBO_WIDTH, FBO_HEIGHT) == TRUE)
	{
		fboResult = initialize_sphere(FBO_WIDTH, FBO_HEIGHT);
	}

	return 0;
}

void printGLInfo(void)
{
	// variable declaration
	GLint numExtentions = 0;
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
		fprintf(gpFile, "%s \n", glGetStringi(GL_EXTENSIONS, i));
	}

	fprintf(gpFile, "****************************************************************************************** \n");


}

BOOL initialize_sphere(GLint textureWidth, GLint textureHeight)
{
	// Function Declarations

	void resize_sphere(int, int);
	void uninitialized_sphere(void);

	//code

	// Vertex Shader

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
		glBindAttribLocation(shaderProgramObject_PV_sphere, AMC_ATTRIBUTE_NORMALS, "aNormal");

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
		glBindAttribLocation(shaderProgramObject_PF_sphere, AMC_ATTRIBUTE_NORMALS, "aNormal");

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

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMALS);

	glBindBuffer(GL_ARRAY_BUFFER, 0);



	glGenBuffers(1, &vbo_texcoords_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_sphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_textures), sphere_textures, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
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

	//FBO related code


	return TRUE;
}

BOOL createFBO(GLint textureWidth, GLint textureHeight)
{
	//check capacity of render buffer 
	GLint maxRenderBufferSize;

	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);
	if (maxRenderBufferSize < textureWidth || maxRenderBufferSize < textureHeight)
	{
		fprintf(gpFile, "Texture sizes are overflow \n");
		return FALSE;
	}

	//create custom frame buffer 
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//create texture for fbo in which we are going to render the sphere
	glGenTextures(1, &textureFBO);
	glBindTexture(GL_TEXTURE_2D, textureFBO);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);

	//attach above textire to framebuffer at color attchment 0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureFBO, 0);

	//now create render buffer to hold depth of custom fbo
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	 
	//set the storage of above render buffer of texture size for depth
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, textureWidth, textureHeight);

	//attach above depth related render buffer to FBO at depth attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);


	//check framebuffer status if successful or not 
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(gpFile, "Framebuffer creation status is not complete \n");
		return FALSE;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return TRUE;
}

void resize_cube(int width, int height)
{
	// code
	if (height <= 0)
	{
		height = 1;
	}
	WinWidth = width;
	WinHeight = height;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix_cube = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);

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

void display_sphere(GLint textureWidth, GLint textureHeight)
{
	void resize_sphere(int, int);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//call resize cube 
	resize_cube(WinWidth, WinHeight);
	//set the clear color to black angain to compensate the change done by display_sphere
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//

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
	translationMatrix = vmath::translate(-1.0f, 0.0f, -6.0f);

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

	//unbind with the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void display_cube(void)
{
	void display_sphere(GLint, GLint);
	void update_sphere(void);
	void resize_sphere(int, int);

	//render fbo scene

	if (fboResult == TRUE)
	{
		display_sphere(FBO_WIDTH, FBO_HEIGHT);
		update_sphere();
	}

	// call resize cube again to compensate the change done by display sphere

	resize_cube(WinWidth, WinHeight);

	//reset the color to white to compensate the change done by display sphere
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObject_cube);

	// cube
	{

		// transformation

		mat4 modelViewMatrix = mat4::identity();

		mat4 translationMatrix = mat4::identity();
		translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);

		mat4 scaleMatrix = mat4::identity();
		scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);

		mat4 rotationMatrix1 = mat4::identity();
		rotationMatrix1 = vmath::rotate(angle_cube, 1.0f, 0.0f, 0.0f);

		mat4 rotationMatrix2 = mat4::identity();
		rotationMatrix2 = vmath::rotate(angle_cube, 0.0f, 1.0f, 0.0f);

		mat4 rotationMatrix3 = mat4::identity();
		rotationMatrix3 = vmath::rotate(angle_cube, 0.0f, 0.0f, 1.0f);

		mat4 rotationMatrix = rotationMatrix1 * rotationMatrix2 * rotationMatrix3;


		modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix ;

		mat4 modelViewProjectionMatrix = mat4::identity();
		modelViewProjectionMatrix = perspectiveProjectionMatrix_cube * modelViewMatrix; // order of multiplication is very important

		// now push modelViewProjectionMatrix in the "uMVPMatrix" of shader , per frame.

		// push above mvp into vertex shader mvpMatrixUniform_cube


		glUniformMatrix4fv(mvpMatrixUniform_cube, 1, GL_FALSE, modelViewProjectionMatrix);

		//for texture

		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture_kundali);
		glBindTexture(GL_TEXTURE_2D, textureFBO);
		glUniform1i(textureSamplerUniform_cube, 0);

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

	SwapBuffers(ghdc);

}
 
void update_cube(void)
{
	//animation

	angle_cube -= 0.01f;
	if (angle_cube <= 0.0f)
	{
		angle_cube += 360.0f;
	}

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

void uninitialized_cube(void)
{
	//  function declarations
	fprintf(gpFile, "****************************************************************************************** \n");
	// code

	if (shaderProgramObject_cube)
	{
		glUseProgram(shaderProgramObject_cube);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_cube, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_cube, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_cube, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_cube);
		shaderProgramObject_cube = 0;
	}

	// for square 
	{

		// delete VBO of position


		if (vbo_position_cube)
		{
			glDeleteBuffers(1, &vbo_position_cube);
			vbo_position_cube = 0;
		}

		// delete VAO

		if (vao_Cube)
		{
			glDeleteVertexArrays(1, &vao_Cube);
			vao_Cube = 0;
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

	
}




